#include <random>
#include <iostream>
#include <thread>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <opencv2/ml.hpp>

#include "util.hpp"

using namespace cv;

void ImageFromDisplay(std::vector<uint8_t>& Pixels, const int& Width, const int& Height, int& BitsPerPixel,
                      const int& startX, const int& startY)
{
    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    // Width = attributes.width;
    // Height = attributes.height;

    std::cout << "height = " << Height << ", width = " << Width << std::endl;

    // these are used for the big image
    XImage* img = XGetImage(display, root, startX, startY, Width, Height, AllPlanes, ZPixmap);
    BitsPerPixel = img->bits_per_pixel;
    Pixels.clear();
    Pixels.resize(Width * Height * 4);

    memcpy(&Pixels[0], img->data, Pixels.size());

    XDestroyImage(img);
    XCloseDisplay(display);
}

int main()
{
    const int sizex = 50;
    const int sizey = 50;

    const int Width = 600;
    const int Height = 630;

    int Bpp = 0;

    std::vector<std::uint8_t> Pixels;
    std::vector<std::uint8_t> squarePixels;

    bool running = true;

    ImageFromDisplay(squarePixels, sizex, sizey, Bpp, 900, 475);
    const Mat birdImg = Mat(sizex, sizey, Bpp > 24 ? CV_8UC4 : CV_8UC3, &squarePixels[0]);

    // imshow("Bird image", birdImg);
    // if ((char)cv::waitKey(0) == 'q') {
    //     cv::destroyAllWindows();
    //     running = false;
    // }

    while (running) {
        ImageFromDisplay(Pixels, Width, Height, Bpp, 650, 170);

        const Mat img = Mat(Height, Width, Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);

        if (img.empty() || birdImg.empty()) {
            std::cerr << "Can't read one of the images" << std::endl;
            return EXIT_FAILURE;
        }

        // alpha channel does not get read => we need to add it
        // cvtColor(birdImg, birdImg, COLOR_BGR2BGRA);

        DEB(img.size());
        DEB(birdImg.size());

        // imshow("Initial image", img);

        DEB(img.type());
        DEB(birdImg.type());

        Mat result;
        const int result_cols = img.cols - birdImg.cols + 1;
        const int result_rows = img.rows - birdImg.rows + 1;
        result.create(result_rows, result_cols, CV_32FC1);

        matchTemplate(img, birdImg, result, TM_SQDIFF);

        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

        rectangle(img, minLoc, Point(minLoc.x + birdImg.cols, minLoc.y + birdImg.rows), Scalar{255, 0, 0}, 2, 8,
                  0);
        imshow("Finished Image", img);

        if ((char)cv::waitKey(25) == 'q') {
            cv::destroyAllWindows();
            running = false;
            break;
        }
    }
    return 0;
}