#ifndef _CV_UTILS_HEADER_HPP_
#define _CV_UTILS_HEADER_HPP_ 1
#pragma once

#include <string>
#include <opencv2/core.hpp>

namespace rmg
{
std::string typeToString(int type)
{
    std::string rv;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U: rv = "8U"; break;
    case CV_8S: rv = "8S"; break;
    case CV_16U: rv = "16U"; break;
    case CV_16S: rv = "16S"; break;
    case CV_32S: rv = "32S"; break;
    case CV_32F: rv = "32F"; break;
    case CV_64F: rv = "64F"; break;
    default: rv = "User"; break;
    }

    rv += "C";
    rv += (chans + '0');

    return rv;
}
} // namespace rmg

#endif //!_CV_UTILS_HEADER_HPP_
