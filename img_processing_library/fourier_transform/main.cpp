#include <iostream>
#include <string>
#include <complex>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

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

cv::Mat computeDFTWrong(const cv::Mat& mat)
{
    assert(mat.type() == CV_64FC1);
    const std::size_t cols = mat.cols;
    const std::size_t rows = mat.rows;

    cv::Mat rv = cv::Mat_<std::complex<double>>(rows, cols);
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            // zk represents a point in the Fourier transform
            std::complex<double> zK{};
            for (std::size_t rK = 0; rK < rows; ++rK) {
                for (std::size_t cK = 0; cK < cols; ++cK) {
                    const double phi = 2. * M_PI *
                                       (((double)cK * (double)c / (double)cols) +
                                        ((double)rK * (double)r / (double)rows));
                    zK += mat.at<double>(r, c) * std::complex<double>(cos(phi), -sin(phi));
                }
            }
            rv.at<std::complex<double>>(r, c) = zK;
        }
    }

    // should have the same size as mat
    assert(rv.size() == mat.size());
    return rv;
}

cv::Mat computeDFT(const cv::Mat& mat)
{
    assert(mat.type() == CV_64FC1);
    const std::size_t cols = mat.cols;
    const std::size_t rows = mat.rows;

    cv::Mat chans[2] = {cv::Mat::zeros(rows, cols, CV_64FC1), cv::Mat::zeros(rows, cols, CV_64FC1)};
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            // zk represents a point in the Fourier transform
            double real{};
            double im{};
            for (std::size_t rK = 0; rK < rows; ++rK) {
                for (std::size_t cK = 0; cK < cols; ++cK) {
                    const double phi = 2. * M_PI *
                                       (((double)cK * (double)c / (double)cols) +
                                        ((double)rK * (double)r / (double)rows));
                    real += mat.at<double>(r, c) * cos(phi);
                    im += mat.at<double>(r, c) * -sin(phi);
                }
            }
            chans[0].at<double>(r, c) = real;
            chans[1].at<double>(r, c) = im;
        }
    }

    cv::Mat rv;
    merge(chans, 2, rv);
    assert(rv.size() == mat.size());

    // should have the same size as mat
    return rv;
}

int main()
{
    const std::string image_path = cv::samples::findFile("man_on_the_moon_unfiltered.jpeg");
    cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE); // CV_8UC1
    if (img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    cv::imshow("Display window", img);
    img.convertTo(img, CV_64F);
    DEB(img.type()); // CV_64FC1

    auto imgFT = computeDFT(img);
    assert(imgFT.size() == img.size());
    DEB(imgFT.type());

    cv::Mat resReal = cv::Mat(imgFT.rows, imgFT.cols, CV_64FC1);
    for (std::size_t i = 0; i < imgFT.rows; i++) {
        for (std::size_t j = 0; j < imgFT.cols; j++) {
            resReal.at<double>(i, j) = imgFT.at<std::complex<double>>(i, j).real();
        }
    }
    // cv::imshow("Miki Fourier Transform ", resReal);

    // TODO: make a function: compute fftfreq
    // f = [0, 1, ...,   n/2-1,     -n/2, ..., -1] / (d*n)   if n is even
    // f = [0, 1, ..., (n-1)/2, -(n-1)/2, ..., -1] / (d*n)   if n is odd
    auto v = rmg::linspace(0., (double)img.rows / 2. - 1., img.rows / 2);
    {
        auto tmp = rmg::linspace(-(double)img.rows / 2., -1., img.rows / 2);
        v.insert(v.end(), tmp.begin(), tmp.end());
    }
    //todo: these seem to be wrong
    // assert(v.size() == img.rows);

    auto u = rmg::linspace(0., (double)img.cols / 2. - 1., img.cols / 2);
    {
        auto tmp = rmg::linspace(-(double)img.cols / 2., -1., img.cols / 2);
        u.insert(u.end(), tmp.begin(), tmp.end());
    }
    //todo: these seem to be wrong
    // assert(u.size() == img.cols);

    // make a meshgrid
    cv::Mat vv = cv::Mat(v.size(), u.size(), CV_64FC1);
    for (std::size_t i = 0; i < v.size(); i++) {
        for (std::size_t j = 0; j < u.size(); j++) { 
            vv.at<double>(i, j) = v[i]; 
        }
    }
    //todo: these seem to be wrong
    // assert(vv.size() == img.size());

    cv::Mat uu = cv::Mat(v.size(), u.size(), CV_64FC1);
    for (std::size_t i = 0; i < v.size(); i++) {
        for (std::size_t j = 0; j < u.size(); j++) { 
            uu.at<double>(i, j) = u[j];
        }
    }
    //todo: these seem to be wrong
    // assert(uu.size() == img.size());

    cv::Mat chans[2] = {cv::Mat::zeros(img.rows, img.cols, CV_64FC1),
                        cv::Mat::zeros(img.rows, img.cols, CV_64FC1)};
    for (std::size_t channel = 0; channel < imgFT.channels(); channel++) {
        for (std::size_t r = 0; r < img.rows; ++r) {
            for (std::size_t c = 0; c < img.cols; ++c) {
                chans[channel].at<double>(r, c) = -4. * M_PI * M_PI *
                                                  (vv.at<double>(r, c) * vv.at<double>(r, c) +
                                                   uu.at<double>(r, c) * uu.at<double>(r, c));
            }
        }
    }
    cv::Mat laplace = cv::Mat(img.rows, img.cols, CV_64FC2);
    merge(chans, 2, laplace);
    assert(laplace.size() == img.size());
    DEB(laplace.type());

    // // cv::imshow("OpenCV Fourier Transform ", fourierTransform);
    // cv::Mat res = imgFT.mul(laplace);
    // cv::Mat inverseTransform;
    // cv::dft(res, inverseTransform, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    // DEB(inverseTransform.size());
    // DEB(inverseTransform.type());

    // // Back to 8-bits
    // cv::Mat finalImage;
    // inverseTransform.convertTo(finalImage, CV_8U);
    // cv::imshow("Final Image", finalImage);

    //*************************** OpenCV Solution ****************************************
    /**The performance of a DFT is dependent of the image size. It tends to be the fastest for image sizes that are
     * multiple of the numbers two, three and five. Therefore, to achieve maximal performance it is generally a
     * good idea to pad border values to the image to get a size with such traits.*/
    cv::Mat fourierTransform;
    cv::dft(img, fourierTransform, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);

    cv::Mat planes[2];
    planes[0] = cv::Mat::zeros(img.rows, img.cols, CV_64FC1); // green channel is set to 0
    planes[1] = cv::Mat::zeros(img.rows, img.cols, CV_64FC1); // red channel is set to 0
    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    cv::split(fourierTransform, planes);            // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    cv::magnitude(planes[0], planes[1], planes[0]); // planes[0] = magnitude
    cv::Mat magI = planes[0];

    magI += cv::Scalar::all(1); // switch to logarithmic scale
    cv::log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    const int cx = magI.cols / 2;
    const int cy = magI.rows / 2;
    //  For visualization purposes we may also rearrange the quadrants of Fourier image so that the origin is at
    //  the image center
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

    imshow("OpenCV spectrum magnitude", magI);

    cv::Mat inverseTransform;
    cv::dft(fourierTransform.mul(laplace), inverseTransform,
            cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    // Back to 8-bits
    cv::Mat finalImage;
    inverseTransform.convertTo(finalImage, CV_8U);
    cv::imshow("OpenCV Final Image", finalImage);

    DEB(imgFT);
    DEB(fourierTransform);

    DEB(imgFT.type());
    DEB(fourierTransform.type());

    int k = cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}
