#ifndef _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_
#define _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_ 1
#pragma once

#include <iostream>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>
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
        conv1 = register_module("conv1",
                                torch::nn::Sequential(torch::nn::Conv2d(3, 32, 5 /*kernel_size*/),
                                                      torch::nn::ELU(torch::nn::ELUOptions().inplace(true)),
                                                      torch::nn::Dropout(torch::nn::DropoutOptions().p(0.1)),
                                                      torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({2, 2})
                                                                                       //    .kernel_size(2)
                                                                                       //    .stride(2)
                                                                                       .padding(0)
                                                                                       .dilation(1)
                                                                                       .ceil_mode(false))));
        //Output is: 

        conv2 = register_module("conv2",
                                torch::nn::Sequential(torch::nn::Conv2d(32, 64, 4 /*kernel_size*/),
                                                      torch::nn::ELU(torch::nn::ELUOptions().inplace(true)),
                                                      torch::nn::Dropout(torch::nn::DropoutOptions().p(0.1)),
                                                      torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({2, 2})
                                                                                       //    .kernel_size(2)
                                                                                       //    .stride(2)
                                                                                       .padding(0)
                                                                                       .dilation(1)
                                                                                       .ceil_mode(false))));

        conv3 = register_module("conv3",
                                torch::nn::Sequential(torch::nn::Conv2d(64, 128, 3 /*kernel_size*/),
                                                      torch::nn::ELU(torch::nn::ELUOptions().inplace(true)),
                                                      torch::nn::Dropout(torch::nn::DropoutOptions().p(0.1)),
                                                      torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({2, 2})
                                                                                       //    .kernel_size(2)
                                                                                       //    .stride(2)
                                                                                       .padding(0)
                                                                                       .dilation(1)
                                                                                       .ceil_mode(false))));

        conv4 = register_module(
                    "conv4",
                    torch::nn::Sequential(
                                torch::nn::Conv2d(
                                            torch::nn::Conv2dOptions({128, 256, 2 /*kernel_size*/}).stride(1)),
                                torch::nn::ELU(torch::nn::ELUOptions().inplace(true)),
                                torch::nn::Dropout(torch::nn::DropoutOptions().p(0.1)),
                                torch::nn::MaxPool2d(torch::nn::MaxPool2dOptions({2, 2})
                                                                 //    .kernel_size(2)
                                                                 //    .stride(2)
                                                                 .padding(0)
                                                                 .dilation(1)
                                                                 .ceil_mode(false))));

        fc1 = register_module(
                    "fc1", torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(230400, 512).bias(true)),
                                                 torch::nn::ELU(torch::nn::ELUOptions().inplace(true)),
                                                 torch::nn::Dropout(torch::nn::DropoutOptions().p(0.1))));

        last = register_module(
                    "last", torch::nn::Sequential(torch::nn::Linear(torch::nn::LinearOptions(512, 136).bias(true)),
                                                  torch::nn::ELU(torch::nn::ELUOptions().inplace(true))));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x)
    {
        DEB(x.scalar_type());
        x = x.to(torch::kFloat32);
        DEB(x.scalar_type());
        x = conv1->forward(x);
        DEB(x.sizes());
        x = conv2->forward(x);
        DEB(x.sizes());
        x = conv3->forward(x);
        DEB(x.sizes());
        x = conv4->forward(x);
        DEB(x.sizes());
        x= x.view({x.size(0), -1 });
        DEB(x.sizes());
        x = fc1->forward(x);
        DEB(x.sizes());
        x = last->forward(x);
        DEB(x.sizes());
        return x;
    }

    // torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

    torch::nn::Sequential conv1{nullptr};
    torch::nn::Sequential conv2{nullptr};
    torch::nn::Sequential conv3{nullptr};
    torch::nn::Sequential conv4{nullptr};

    torch::nn::Sequential fc1{nullptr};
    torch::nn::Sequential fc2{nullptr};
    torch::nn::Sequential fc3{nullptr};

    torch::nn::Sequential last{nullptr};

    // torch::nn::Conv2d conv{nullptr};
    // torch::nn::ELU elu{nullptr};
};

class KeypointsModelTest : public torch::nn::Module
{
public:
    KeypointsModelTest()
    {
        conv = register_module("conv", torch::nn::Conv2d(3, 32, 5 /*kernel_size*/));
        elu = register_module("elu", torch::nn::ELU(torch::nn::ELUOptions().inplace(true)));
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
    torch::nn::ELU elu{nullptr};
};

#endif

/**
 * @brief RuntimeError: expected scalar type Byte but found Float
The error message might be a bit confusing, as PyTorch assumes that the first argument to an operation has the
desired dtype (which is a ByteTensor in torch._convolution(input, self.weight, ...)) and complains about the
self.weight, which is a FloatTensor.
 *
 */