#ifndef _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_
#define _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_ 1
#pragma once

#include <iostream>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>

#include "facial_keypoints.hpp"

/**
1. This network takes in a square (same width and height), grayscale image as input
2. It ends with a linear layer that represents the keypoints
it's suggested that you make this last layer output 30 values, 2 for each of the 15 keypoint (x, y) pairs

Note that among the layers to add, consider including:
maxpooling layers, multiple conv layers, fully-connected layers, and other layers (such as dropout or
batch normalization) to avoid overfitting.
*/
class KeypointsModel : public torch::nn::Module
{
public:
    KeypointsModel()
    {
        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(
                                                  torch::data::datasets::FaceLandmarksDataset::ImageChannels, 8,
                                                  3 /*kernel_size*/)
                                                  .stride(2)
                                                  .bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(8)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(8, 16, 3 /*kernel_size*/).stride(2).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(16)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(16, 16, 3 /*kernel_size*/).stride(1).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(16)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(16, 32, 3 /*kernel_size*/).stride(2).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(32)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(32, 32, 3 /*kernel_size*/).stride(1).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(32)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(32, 64, 3 /*kernel_size*/).stride(2).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(64)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 64, 3 /*kernel_size*/).stride(1).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(64)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3 /*kernel_size*/).stride(2).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(128)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        convLayers_.push_back(torch::nn::Sequential(
                    torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 128, 3 /*kernel_size*/).stride(1).bias(false)),
                    torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(128)
                                                       .eps(1e-5)
                                                       .momentum(0.1)
                                                       .affine(true)
                                                       .track_running_stats(true)),
                    torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));

        fcLayers_.push_back(
                    torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(15488, 4096).bias(true)),
                                          torch::nn::BatchNorm1d(torch::nn::BatchNorm1dOptions(4096)
                                                                             .eps(1e-5)
                                                                             .momentum(0.1)
                                                                             .affine(true)
                                                                             .track_running_stats(true)),
                                          torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));
        fcLayers_.push_back(
                    torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(4096, 1024).bias(true)),
                                          torch::nn::BatchNorm1d(torch::nn::BatchNorm1dOptions(1024)
                                                                             .eps(1e-5)
                                                                             .momentum(0.1)
                                                                             .affine(true)
                                                                             .track_running_stats(true)),
                                          torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));
        fcLayers_.push_back(
                    torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(1024, 512).bias(true)),
                                          torch::nn::BatchNorm1d(torch::nn::BatchNorm1dOptions(512)
                                                                             .eps(1e-5)
                                                                             .momentum(0.1)
                                                                             .affine(true)
                                                                             .track_running_stats(true)),
                                          torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));
        last = register_module(
                    "last", torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(512, 136).bias(true)),
                                                  torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true))));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x)
    {
        x = x.to(torch::kFloat32);
        for (auto& layer : convLayers_) x = layer->forward(x);
        x = x.view({x.size(0), -1});
        // DEB(x.sizes());
        for (auto& layer : fcLayers_) x = layer->forward(x);
        // DEB(x.sizes());
        x = last->forward(x);
        // DEB(x.sizes());
        return x;
    }

private:
    std::vector<torch::nn::Sequential> convLayers_{};
    std::vector<torch::nn::Sequential> fcLayers_{};

    torch::nn::Sequential last{nullptr};
};

class KeypointsModelTest : public torch::nn::Module
{
public:
    KeypointsModelTest()
    {
        conv = register_module("conv", torch::nn::Conv2d(3, 32, 5 /*kernel_size*/));
        elu = register_module("elu", torch::nn::ReLU(torch::nn::ReLUOptions().inplace(true)));
    }

    torch::Tensor forward(torch::Tensor x)
    {
        DEB(x.scalar_type());
        x = x.to(torch::kFloat32);
        DEB(x.scalar_type());
        x = conv->forward(x);
        DEB(x.sizes());
        return x;
    }

    torch::nn::Conv2d conv{nullptr};
    torch::nn::ReLU elu{nullptr};
};

#endif

/**
 * @brief RuntimeError: expected scalar type Byte but found Float
The error message might be a bit confusing, as PyTorch assumes that the first argument to an operation has the
desired dtype (which is a ByteTensor in torch._convolution(input, self.weight, ...)) and complains about the
self.weight, which is a FloatTensor.
 *
 */