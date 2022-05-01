#include <iostream>
#include <string>
#include <complex>
#include <thread>
#include <cmath>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/quality/qualitymse.hpp>

#include "util.hpp"
#include "cv_utils.hpp"

/******************************************************************************************************************
 * !Implement fourier transform filter!
 *
 * The result of the transformation is complex numbers. Displaying this is possible either via a real image and a
 * complex image or via a magnitude and a phase image.
 *
 * However, throughout the image processing algorithms only the magnitude image is interesting as this contains all
 * the information we need about the images geometric structure.
 *****************************************************************************************************************/

cv::Mat computeDFTNaive(const cv::Mat& mat)
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
                    const double phi =
                                2. * M_PI *
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

void rearrangeImage(cv::Mat& mat)
{
    // For visualization purposes we may also rearrange the quadrants of Fourier image so that the origin is at
    // the image center
    const int cx = mat.cols / 2;
    const int cy = mat.rows / 2;
    // No data is copied by these constructors. Instead, the header pointing to m data or its sub-array is
    // constructed and associated with it
    cv::Mat q0(mat, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(mat, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(mat, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(mat, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    // swap quadrants (Top-Left with Bottom-Right)
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

cv::Mat displayFreqImage(const cv::Mat& mat, const char* name, bool rearrange = true)
{
    cv::Mat planes[2] = {cv::Mat::zeros(mat.rows, mat.cols, CV_64FC1),
                         cv::Mat::zeros(mat.rows, mat.cols, CV_64FC1)};

    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    cv::split(mat, planes);                         // planes[0] = Re(DFT(img)), planes[1] = Im(DFT(img))
    cv::magnitude(planes[0], planes[1], planes[0]); // planes[0] = magnitude
    cv::Mat magI = planes[0];

    magI += cv::Scalar::all(1); // switch to logarithmic scale
    cv::log(magI, magI);

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    if (rearrange) rearrangeImage(magI);

    // Transform the matrix with float values into a viewable image form (float between values 0 and 1).
    cv::normalize(magI, magI, 0, 1, cv::NORM_MINMAX);

    cv::imshow(name, magI);

    return magI;
}

/**
 * @brief Discrete Fourier Transform sample frequencies.
 *
 * @param n Window length
 * @param d Sample spacing (inverse of the sampling rate). Defaults to 1.
 * @return std::vector<double> contains the frequency bin centers in cycles per unit of the sample spacing (with
 * zero at the start). For instance, if the sample spacing is in seconds, then the frequency unit is cycles/second
 */
std::vector<double> computeFFTfreq(std::size_t n, double d = 1.0)
{
    // f = [0, 1, ...,   n/2-1,     -n/2, ..., -1] / (d*n)   if n is even
    // f = [0, 1, ..., (n-1)/2, -(n-1)/2, ..., -1] / (d*n)   if n is odd

    auto rv = rmg::linspace(0., (double)n / 2. - 1., d * n / 2);
    const auto secondPart = rmg::linspace(-(double)n / 2., -1., d * n / 2);
    rv.insert(rv.end(), secondPart.begin(), secondPart.end());

    assert(rv.size() == n);
    return rv;
}

class Filter
{
public:
    virtual cv::Mat createFilter(std::size_t rows, std::size_t cols) = 0;
    virtual void doFiltering(const cv::Mat& img) = 0;
};

class FrequencyFilter : public Filter
{
public:
    void doFiltering(const cv::Mat& img) override
    {
        //***** Band reject Filtering ****************************//
        cv::Mat imgFT;
        cv::dft(img, imgFT, /* cv::DFT_SCALE | */ cv::DFT_COMPLEX_OUTPUT);
        assert(imgFT.size() == img.size());

        cv::Mat magI = displayFreqImage(imgFT, "Initial spectrum magnitude");

        rearrangeImage(imgFT);

        const auto filter = this->createFilter(img.rows, img.cols);
        // DEB(filter.size());
        // DEB(filter.type());

        // DEB(imgFT.size());
        // DEB(imgFT.type());
        cv::Mat imgFilteredFT = imgFT.mul(filter);
        displayFreqImage(imgFilteredFT, "Filtered spectrum magnitude", false);

        rearrangeImage(imgFilteredFT);

        cv::Mat imgFiltered;
        cv::dft(imgFilteredFT, imgFiltered, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

        // Back to 8-bits
        cv::Mat finalImage;
        imgFiltered.convertTo(finalImage, CV_8U);
        cv::imshow("OpenCV Final Image", finalImage);
    }
};

class LaplaceFilter : public FrequencyFilter
{
public:
    explicit LaplaceFilter(bool naive) : naive_(naive) {}

    cv::Mat createFilter(std::size_t rows, std::size_t cols) override
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

    void doFiltering(const cv::Mat& img) override
    {
        cv::Mat laplace = createFilter(img.rows, img.cols);
        if (naive_)
            naiveImpl(img, laplace);
        else
            openCvImpl(img, laplace);
    }

private:
    void naiveImpl(const cv::Mat& img, const cv::Mat& laplace)
    {
        cv::Mat imgFT = computeDFTNaive(img);
        assert(imgFT.size() == img.size());
        // DEB(imgFT.type()); //CV_64FC2

        displayFreqImage(imgFT, "Laplace Naive impl spectrum magnitude");

        cv::Mat imgFiltered;
        cv::dft(imgFT.mul(laplace), imgFiltered, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

        // Back to 8-bits
        cv::Mat finalImage;
        imgFiltered.convertTo(finalImage, CV_8U);
        cv::imshow("Laplace Naive impl Final Image", finalImage);
    }

    void openCvImpl(const cv::Mat& img, const cv::Mat& laplace)
    {
        //*************************** OpenCV Solution ****************************************
        /**
         * The performance of a DFT is dependent of the image size. It tends to be the fastest for image sizes that
         * are multiple of the numbers two, three and five. Therefore, to achieve maximal performance it is
         * generally a good idea to pad border values to the image to get a size with such traits.
         */
        cv::Mat imgFT;
        cv::dft(img, imgFT, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
        assert(imgFT.size() == img.size());

        displayFreqImage(imgFT, "Laplace OpenCV impl spectrum magnitude");

        // TODO: cv::DFT_SCALE should not be used in both places
        cv::Mat imgFiltered;
        cv::dft(imgFT.mul(laplace), imgFiltered, cv::DFT_SCALE | cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

        // Back to 8-bits
        cv::Mat finalImage;
        imgFiltered.convertTo(finalImage, CV_8U);
        cv::imshow("Laplace OpenCV impl Final Image", finalImage);
    }

private:
    bool naive_{false};
};

/**
 * @brief filter with radius D, order n
 */
class LowHighpassFilter : public FrequencyFilter
{
public:
    explicit LowHighpassFilter(bool highpass) : highpass_(highpass) {}

    cv::Mat createFilter(std::size_t rows, std::size_t cols) override
    {
        auto dft_Filter = cv::Mat(rows, cols, CV_64F, cv::Scalar::all(0));

        const auto centre = cv::Point(rows / 2, cols / 2);

        // based on the forumla in the IP notes (p. 130 of 2009/10 version)
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                const auto dist = (double)std::sqrt(std::pow((i - centre.x), 2.) +
                                               std::pow((double)(j - centre.y), 2.0));
                dft_Filter.at<double>(i, j) =
                            (double)(1. / (1. + std::pow((double)(dist / radius_), (double)(2. * order_))));
            }
        }

        if (highpass_) {
            dft_Filter = cv::Scalar::all(1) - dft_Filter;
            // cv::imshow("highpass_filter", dft_Filter);
        }

        const cv::Mat toMerge[] = {dft_Filter, dft_Filter};
        cv::merge(toMerge, 2, dft_Filter);

        return dft_Filter;
    }

private:
    bool highpass_{false};
    int order_ = 10;
    int radius_ = 50;
};

//***** Band reject Filtering ****************************//
class BandRejectFilter : public FrequencyFilter
{
public:
    explicit BandRejectFilter(cv::Mat img) : img_(img)
    {
        img_.convertTo(img_, CV_64F);
        maxWidth = (rmg::max(img_.rows, img_.cols));
        maxRadius = (rmg::max(img_.rows, img_.cols));
    }

    /**
     * @brief create a 2-channel band reject filter with radius D and width W
     */
    cv::Mat createFilter(std::size_t rows, std::size_t cols) override
    {
        auto filter = cv::Mat(rows, cols, CV_64F, cv::Scalar::all(0));

        const auto centre = cv::Point(rows / 2, cols / 2);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                // Gonzalez Woods, "Digital Image Processing 2nd Edition, page 244
                // D(u,v) -> dist
                // D_0 -> D

                const double dist = std::sqrt((double)(i - centre.x) * (double)(i - centre.x) +
                                              (double)(j - centre.y) * (double)(j - centre.y));

                if (type_ == Type::Butterworth) {
                    filter.at<double>(i, j) =
                                (double)(1 / (1 + pow((double)(dist * width_) / (pow((double)dist, 2) - radius_ * radius_),
                                                      (double)(2 * order_))));
                } else if (type_ == Type::Ideal) {
                    if (dist >= radius_ - width_ / 2. && dist <= radius_ + width_ / 2.) {
                        filter.at<double>(i, j) = 0;
                        continue;
                    }
                    filter.at<double>(i, j) = 1;
                }
            }
        }

        const cv::Mat toMerge[] = {filter, filter};
        cv::merge(toMerge, 2, filter);

        return filter;
    }

    /**
     * @brief 
     * 
     * @param pos trackbar position 
     * @param voidBandRejectFilter userdata
     */
    static void onRadiusTrackbar(int pos, void* voidBandRejectFilter)
    {
        BandRejectFilter* filter = reinterpret_cast<BandRejectFilter*>(voidBandRejectFilter);
        assert(filter->radius_ == pos);
        filter->doFiltering(filter->img_);
    }

    static void onWidthTrackbar(int pos, void* voidBandRejectFilter)
    {
        BandRejectFilter* filter = reinterpret_cast<BandRejectFilter*>(voidBandRejectFilter);
        assert(filter->width_ == pos);
        filter->doFiltering(filter->img_);
    }

    static void onOrderChange(int pos, void* voidBandRejectFilter)
    {
        BandRejectFilter* filter = reinterpret_cast<BandRejectFilter*>(voidBandRejectFilter);
        assert(filter->order_ == pos);
        filter->doFiltering(filter->img_);
    }

public:
    enum class Type { Ideal, Gauss, Butterworth };

    Type type_{Type::Butterworth};

    int maxWidth = 150;
    int maxRadius = 300;

    int order_ = 10;

    int radius_ = 50;
    int width_ = 10;
    cv::Mat img_;
};

int main()
{
    const std::string image_path = cv::samples::findFile("man_on_the_moon_unfiltered.jpeg");
    cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE); // CV_8UC1
    const std::size_t rows = img.rows;
    const std::size_t cols = img.cols;
    if (img.empty()) {
        std::cerr << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window

    BandRejectFilter bandRejectFilter(img);
    const std::string radiusTrackbarName = "Band-reject radius";
    cv::createTrackbar(radiusTrackbarName.c_str(), "Display window", &bandRejectFilter.radius_,
                       bandRejectFilter.maxRadius, BandRejectFilter::onRadiusTrackbar, &bandRejectFilter);

    const std::string widthTrackbarName = "Band-reject width";
    cv::createTrackbar(widthTrackbarName.c_str(), "Display window", &bandRejectFilter.width_,
                       bandRejectFilter.maxWidth, BandRejectFilter::onWidthTrackbar, &bandRejectFilter);

    const std::string orderTrackbarName = "Butterworth Band-reject order";
    cv::createTrackbar(orderTrackbarName.c_str(), "Display window", &bandRejectFilter.order_, 10,
                       BandRejectFilter::onOrderChange, &bandRejectFilter);

    cv::imshow("Display window", img);
    img.convertTo(img, CV_64F);
    // DEB(img.type()); // CV_64FC1

    DEB(rmg::typeToString(img.type()));

    // LaplaceFilter laplaceFilterNaive(true);
    // LaplaceFilter laplaceFilterOpenCV(false);
    // std::thread th1(&LaplaceFilter::doFiltering, laplaceFilterNaive, img);
    // std::thread th2(&LaplaceFilter::doFiltering, laplaceFilterOpenCV, img);
    // th1.join();
    // th2.join();

    int k = cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}
