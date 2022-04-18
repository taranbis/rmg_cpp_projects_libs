#include <iostream>
#include <string>
#include <complex>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/quality/qualitymse.hpp>

#include "util.hpp"

/******************************************************************************************************************
 * !Implement fourier transform filter!
 * 
 * The result of the transformation is complex numbers. Displaying this is possible either via a real image and a
 * complex image or via a magnitude and a phase image.
 *
 * However, throughout the image processing algorithms only the magnitude image is interesting as this contains all
 * the information we need about the images geometric structure.
 *****************************************************************************************************************/

cv::Mat computeDFT(const cv::Mat& mat)
{
    // Done: parallelize this
    assert(mat.type() == CV_64FC1);
    const std::size_t cols = mat.cols;
    const std::size_t rows = mat.rows;

    cv::Mat rv = cv::Mat_<std::complex<double>>(rows, cols);

    const auto computeValueAtIdx = [&](std::size_t k) {
        for (std::size_t l = 0; l < cols; ++l) {
            for (std::size_t i = 0; i < rows; ++i) {
                for (std::size_t j = 0; j < cols; ++j) {
                    const double phi = 2. * M_PI *
                                (((double)j * (double)l / (double)cols) + ((double)i * (double)k / (double)rows));
                    rv.at<std::complex<double>>(k, l) +=
                                mat.at<double>(i, j) * std::complex<double>(cos(phi), -sin(phi));
                }
            }
            rv.at<std::complex<double>>(k, l) /= (double)(rows * cols);
        }
    };

    std::vector<std::thread> threads;
    for (std::size_t k = 0; k < rows; ++k) threads.push_back(std::thread(computeValueAtIdx, k));

    for (auto& thread : threads) thread.join();

    // should have the same size as mat
    assert(rv.size() == mat.size());
    return rv;
}

void displayFreqImage(const cv::Mat& mat, const char* name)
{
    cv::Mat planes[2] = {cv::Mat::zeros(mat.rows, mat.cols, CV_64FC1), cv::Mat::zeros(mat.rows, mat.cols, CV_64FC1)};

    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    cv::split(mat, planes);                         // planes[0] = Re(DFT(img)), planes[1] = Im(DFT(img))
    cv::magnitude(planes[0], planes[1], planes[0]); // planes[0] = magnitude
    cv::Mat magI = planes[0];

    magI += cv::Scalar::all(1); // switch to logarithmic scale
    cv::log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // For visualization purposes we may also rearrange the quadrants of Fourier image so that the origin is at
    // the image center
    const int cx = magI.cols / 2;
    const int cy = magI.rows / 2;
    // No data is copied by these constructors. Instead, the header pointing to m data or its sub-array is
    // constructed and associated with it
    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    // swap quadrants (Top-Left with Bottom-Right)
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Transform the matrix with float values into a viewable image form (float between values 0 and 1).
    cv::normalize(magI, magI, 0, 1, cv::NORM_MINMAX);

    cv::imshow(name, magI);
}

void naiveImpl(const cv::Mat& img, const cv::Mat& laplace)
{
    cv::Mat imgFT = computeDFT(img);
    assert(imgFT.size() == img.size());
    // DEB(imgFT.type()); //CV_64FC2

    displayFreqImage(imgFT, "Naive spectrum magnitude");

    cv::Mat imgFiltered;
    cv::dft(imgFT.mul(laplace), imgFiltered, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    // Back to 8-bits
    cv::Mat finalImage;
    imgFiltered.convertTo(finalImage, CV_8U);
    cv::imshow("Miki Final Image", finalImage);
}

void openCvImpl(const cv::Mat& img, const cv::Mat& laplace)
{
    //*************************** OpenCV Solution ****************************************
    /**The performance of a DFT is dependent of the image size. It tends to be the fastest for image sizes that are
     * multiple of the numbers two, three and five. Therefore, to achieve maximal performance it is generally a
     * good idea to pad border values to the image to get a size with such traits.
     */
    cv::Mat imgFT;
    cv::dft(img, imgFT, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
    assert(imgFT.size() == img.size());

    displayFreqImage(imgFT, "OpenCV spectrum magnitude");

    // TODO: cv::DFT_SCALE should not be used in both places
    cv::Mat imgFiltered;
    cv::dft(imgFT.mul(laplace), imgFiltered, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    // Back to 8-bits
    cv::Mat finalImage;
    imgFiltered.convertTo(finalImage, CV_8U);
    cv::imshow("OpenCV Final Image", finalImage);
}

std::vector<double> computeFFTfreq(std::size_t n, double d = 1.0)
{
    // f = [0, 1, ...,   n/2-1,     -n/2, ..., -1] / (d*n)   if n is even
    // f = [0, 1, ..., (n-1)/2, -(n-1)/2, ..., -1] / (d*n)   if n is odd

    auto rv = rmg::linspace(0., (double)n / 2. - 1., d * n / 2);
    auto secondPart = rmg::linspace(-(double)n / 2., -1., d * n / 2);
    rv.insert(rv.end(), secondPart.begin(), secondPart.end());

    assert(rv.size() == n);
    return rv;
}

cv::Mat computeLaplaceFilter(std::size_t rows, std::size_t cols)
{
    auto v = computeFFTfreq(rows);
    auto u = computeFFTfreq(cols);

    // create a meshgrid
    auto vv = cv::Mat(v.size(), u.size(), CV_64FC1);
    auto uu = cv::Mat(v.size(), u.size(), CV_64FC1);
    for (std::size_t i = 0; i < v.size(); i++) {
        for (std::size_t j = 0; j < u.size(); j++) {
            vv.at<double>(i, j) = v[i];
            uu.at<double>(i, j) = u[j];
        }
    }

    cv::Mat chans[2] = {cv::Mat::zeros(rows, cols, CV_64FC1), cv::Mat::zeros(rows, cols, CV_64FC1)};
    for (std::size_t channel = 0; channel < 2; channel++) {
        for (std::size_t r = 0; r < rows; ++r) {
            for (std::size_t c = 0; c < cols; ++c) {
                chans[channel].at<double>(r, c) = -4. * M_PI * M_PI *
                                                  (vv.at<double>(r, c) * vv.at<double>(r, c) +
                                                   uu.at<double>(r, c) * uu.at<double>(r, c));
            }
        }
    }
    cv::Mat laplace = cv::Mat(rows, cols, CV_64FC2);
    merge(chans, 2, laplace);
    assert(laplace.rows == rows);
    assert(laplace.cols == cols);
    DEB(laplace.type());

    return laplace;
}

int main()
{
    const std::string image_path = cv::samples::findFile("bricks.png");
    cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE); // CV_8UC1
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    cv::imshow("Display window", img);
    img.convertTo(img, CV_64F);
    DEB(img.type()); // CV_64FC1

    cv::Mat laplace = computeLaplaceFilter(img.rows, img.cols);
    
    std::thread th1(naiveImpl, img, laplace);
    std::thread th2(openCvImpl, img, laplace);
    th1.join(); th2.join();

    int k = cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}
