#ifndef _CV_UTILS_HEADER_HPP_
#define _CV_UTILS_HEADER_HPP_ 1
#pragma once

#include <string>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/quality/qualitymse.hpp>

namespace rmg
{
void printType(const cv::Mat& mat)
{
    const int type = mat.type();

    std::string rv, expl;
    const uchar depth = type & CV_MAT_DEPTH_MASK;
    const uchar chans = 1 + (type >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  rv = "8U";   expl = "Mat.at<uchar>(y,x)"; break;  
        case CV_8S:  rv = "8S";   expl = "Mat.at<schar>(y,x)"; break;  
        case CV_16U: rv = "16U";  expl = "Mat.at<ushort>(y,x)"; break; 
        case CV_16S: rv = "16S";  expl = "Mat.at<short>(y,x)"; break; 
        case CV_32S: rv = "32S";  expl = "Mat.at<int>(y,x)"; break; 
        case CV_32F: rv = "32F";  expl = "Mat.at<float>(y,x)"; break; 
        case CV_64F: rv = "64F";  expl = "Mat.at<double>(y,x)"; break; 
        default:     rv = "User"; expl = "Mat.at<UKNOWN>(y,x)"; break; 
    }
    rv += "C";
    rv += (chans + '0');
    std::cout << "Mat is of type " << rv << " and should be accessed with " << expl << std::endl;
}

cv::Mat readImg(const std::string& path)
{
    const std::string imagePath = cv::samples::findFile(path);
    cv::Mat img = cv::imread(imagePath); // CV_8UC1
    if (img.empty()) std::cerr << "Could not read the image: " << imagePath << std::endl;

    return img;
}

//TODO: implement concepts for this + maybe modify it
template<class Array>
static void displayPoints(cv::Mat img, Array arr)
{
    for (int i = 0; i < arr.size(); ++i) {
        int x = arr[x][0];
        int y = arr[i][1];
        cv::circle(img, cv::Point{x, y}, 1, cv::Scalar{0, 0, 255}, 5);
    }

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create Window
    cv::imshow("Display window", img);

    int k = cv::waitKey(0); // Wait for a keystroke in the window
}
} // namespace rmg

#endif //!_CV_UTILS_HEADER_HPP_
