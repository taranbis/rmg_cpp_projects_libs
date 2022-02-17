#include <random>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <opencv2/ml.hpp>

#include "util.hpp"

#include "utils2048.hpp"
#include "monte_carlo_search.hpp"
//! we can always add 2 equal values
// once we obtain a value score increases by the value we obtain
// 1.
// 2.

using namespace cv;

void ImageFromDisplay(std::vector<uint8_t>& Pixels, const int& Width, const int& Height, int& BitsPerPixel,
                      int startX, int startY)
{
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    // Width = attributes.width;
    // Height = attributes.height;

    std::cout << "height = " << Height << ", width = " << Width << std::endl;

    // these are used for the big image
    // XImage* img = XGetImage(display, root, 600, 100, Width, Height, AllPlanes, ZPixmap);
    XImage* img = XGetImage(display, root, startX, startY, Width, Height, AllPlanes, ZPixmap);
    BitsPerPixel = img->bits_per_pixel;
    Pixels.clear();
    Pixels.resize(Width * Height * 4);

    memcpy(&Pixels[0], img->data, Pixels.size());

    XDestroyImage(img);
    XCloseDisplay(display);
}

void PreProcessImage(Mat* inImage, Mat* outImage, int sizex, int sizey)
{
    Mat grayImage, blurredImage, thresholdImage, contourImage, regionOfInterest;

    std::vector<std::vector<Point>> contours;

    cvtColor(*inImage, grayImage, COLOR_BGR2GRAY);

    GaussianBlur(grayImage, blurredImage, Size(5, 5), 2, 2);
    adaptiveThreshold(blurredImage, thresholdImage, 255, 1, 1, 11, 2);

    thresholdImage.copyTo(contourImage);

    findContours(contourImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    int idx = 0;
    size_t area = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        if (area < contours[i].size()) {
            idx = i;
            area = contours[i].size();
        }
    }

    Rect rec = boundingRect(contours[idx]);

    regionOfInterest = thresholdImage(rec);

    resize(regionOfInterest, *outImage, Size(sizex, sizey));
}

int main()
{
    // FastRandomGenerator randomGen;
    // std::cout << randomGen.generate(16) << std::endl;

    // Board<4,4> board;
    // Game game;
    // int Width = 700;
    // int Height = 750;
    const int Width = 600;
    const int Height = 550;
    int Bpp = 0;

    const int sizex = 103;
    const int sizey = 103;
    std::vector<std::uint8_t> Pixels;
    std::vector<std::uint8_t> squarePixels;

    bool running = true;

    // while (running) {
    ImageFromDisplay(Pixels, Width, Height, Bpp, 650, 300);
    ImageFromDisplay(squarePixels, sizex, sizey, Bpp, 1081, 345);

    const std::vector<int> possibleNumbers{2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    std::vector<cv::Mat> searchImages;

    for(const int& number : possibleNumbers){
        // std::aot
        const std::string imgName = std::to_string(number) + ".png";
        const cv::Mat pngImg = imread(imgName, IMREAD_UNCHANGED);
        if (pngImg.empty()) {
            std::cerr << "Could not read image: " << imgName << std::endl;
            return EXIT_FAILURE;
        }
        searchImages.emplace_back(pngImg);
    }

    const cv::Mat searchImage = searchImages[2];
    // while(running){
    // if (Width && Height) {
    cv::Mat img = Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);
    cv::Mat smallImgEmpty = Mat(sizex, sizey, Bpp > 24 ? CV_8UC4 : CV_8UC3, &squarePixels[0]);

    // TODO: add all other images here
    if (img.empty()) {
        std::cerr << "Can't read one of the images" << std::endl;
        return EXIT_FAILURE;
    }

    // alpha channel does not get read => we need to add it
    cvtColor(smallImgEmpty, smallImgEmpty, COLOR_BGR2BGRA);
    cvtColor(searchImage, searchImage, COLOR_BGR2BGRA); // opacity is set to 255

    DEB(smallImgEmpty.size());
    DEB(searchImage.size());

    imshow("Initial image", img);
    imshow("Small image", searchImage);

    DEB(searchImage.type());
    DEB(img.type());

    cv::Mat result;
    const int result_cols = img.cols - searchImage.cols + 1;
    const int result_rows = img.rows - searchImage.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);

    matchTemplate(img, searchImage, result, TM_CCOEFF_NORMED /* TM_SQDIFF */);

    double minVal;
    // double maxVal;
    // Point minLoc;
    // Point maxLoc;
    minMaxLoc(result, &minVal /* , &maxVal, &minLoc, &maxLoc, Mat() */);

    //! be careful with conversions. keep it in float to do floating point stuff
    const double threshold = 0.75;
    const cv::Mat thresholdImage = result >= threshold;

    imshow("thresholdImage", thresholdImage);
    imshow("Result", result);

    auto isIntersecting = [](cv::Point bottomLeft1, cv::Point topRight1, cv::Point bottomLeft2,
                             cv::Point topRight2) {
        if (topRight1.x < bottomLeft2.x || bottomLeft1.x > topRight2.x) return false; // means they are intersecting
        if (topRight1.y < bottomLeft2.y || bottomLeft1.y > topRight2.y) return false; // means they are intersecting
        return true;
    };

    std::vector<cv::Point> matchingPoints{};
    for (int r = 0; r < thresholdImage.rows; r++) {
        for (int c = 0; c < thresholdImage.cols; c++) {
            if (thresholdImage.at<uchar>(r, c) > 0) {
                bool intersected = false;
                for (const cv::Point& matchingPoint : matchingPoints) {
                    if (isIntersecting(matchingPoint, {matchingPoint.x + sizex, matchingPoint.y + sizey}, {c, r}, {c + sizex, r + sizey})) {
                        intersected = true;
                        break;
                    }
                }
                if (!intersected) {
                    matchingPoints.emplace_back(cv::Point{c, r});
                    rectangle(img, cv::Point(c, r), cv::Point(c + sizex, r + sizey), cv::Scalar::all(0), 2, 8, 0);
                    DEB(cv::Point(c, r));
                }
            }
        }
    }

    DEB(matchingPoints.size());
    imshow("Finished Image", img);
    imshow("Result window", result);
    // return;
    // };

    if ((char)cv::waitKey(0) == 'q') {
        cv::destroyAllWindows();
        running = false;
        // break;
    }
    // }
    return 0;
}