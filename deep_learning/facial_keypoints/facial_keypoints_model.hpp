#ifndef _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_
#define _FACIAL_KEYPOINTS_MODEL_HEADER_HPP_ 1
#pragma once

#include <iostream>

#include <torch/torch.h>
#include <torch/csrc/autograd/variable.h>
#include <torch/csrc/autograd/function.h>

// Define a new Module.
class KeypointsModel : torch::nn::Module
{
public:
    KeypointsModel()
    {
        // Construct and register three Linear submodules.
        fc1 = register_module("fc1", torch::nn::Linear(784, 64));
        fc2 = register_module("fc2", torch::nn::Linear(64, 32));
        fc3 = register_module("fc3", torch::nn::Linear(32, 10));

        //input_channels, output_channels, kernel_size
        conv = register_module("conv", torch::nn::Conv2d(1, 32, 5));
        elu = register_module("elu", torch::nn::ELU(torch::nn::ELUOptions().inplace(true)));

        // conv1 = torch::nn::Sequential(torch::nn::Conv2d(1, 32, 5 /*kernel_size*/), torch::nn::ELU(true),
        //                               torch::nn::Dropout(0.1), torch::nn::MaxPool2d(2, 2, 0, 1, false));
    }

    // Implement the Net's algorithm.
    torch::Tensor forward(torch::Tensor x)
    {
        // Use one of many tensor manipulation functions.
        x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
        x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
        x = torch::relu(fc2->forward(x));
        x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
        return x;
    }

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

    torch::nn::Sequential conv1{nullptr};

    torch::nn::Conv2d conv{nullptr};
    
    torch::nn::ELU elu{nullptr};
};

#endif