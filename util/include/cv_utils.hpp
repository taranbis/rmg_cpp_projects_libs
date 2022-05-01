#ifndef _CV_UTILS_HEADER_HPP_
#define _CV_UTILS_HEADER_HPP_ 1
#pragma once

#include <string>
#include <iostream>

#include <opencv2/core.hpp>

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
} // namespace rmg

#endif //!_CV_UTILS_HEADER_HPP_
