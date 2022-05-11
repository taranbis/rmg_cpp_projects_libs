#ifndef _UTILS_HEADER_HPP_
#define _UTILS_HEADER_HPP_ 1
#pragma once

#include <string>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/quality/qualitymse.hpp>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>

namespace rmg
{
torch::Tensor cvMatToTorchTensor(const cv::Mat& mat)
{
    //! doesn't take ownership
    torch::Tensor tensor = torch::from_blob(mat.data, {mat.rows, mat.cols, mat.channels()}, at::kByte);
    return tensor.to(torch::kFloat32);

    //* maybe also possible (didn't get it working)
    // Tensor tensor = torch::empty({img.rows*img.cols * img.channels()}, torch::kByte);
    // std::memcpy(tensor.data_ptr(), img.data, sizeof(float)*tensor.numel());
}

cv::Mat torchTensorToCvMat(const torch::Tensor& tensor)
{
    //depends on the input: apply tenson.view({512, 512, 3}) or tensor.reshape({512, 512, 3})
    int64_t height = tensor.size(0);
    int64_t width = tensor.size(1);
    return cv::Mat(cv::Size(width, height), CV_8UC3, tensor.data_ptr<uchar>());
}
} // namespace rmg

#endif //!_UTILS_HEADER_HPP_

