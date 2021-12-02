#ifndef _SEQUENTIAL_HEADER_HPP
#define _SEQUENTIAL_HEADER_HPP 1
#pragma once

#include <memory>

#include "util.hpp"

template <typename NumericType, size_t batchDim, size_t inDim, size_t outDim>
class Sequential
{
private:
    std::vector<std::unique_ptr<Layer<double>>> layers_;

public:
    Sequential& operator<<(std::unique_ptr<Layer<NumericType>>&& other)
    {
        layers_.emplace_back(std::move(other));
        return *this;
    }

    Eigen::MatrixXd operator()(Eigen::MatrixXd x)
    {
        Eigen::MatrixXd res = x;

        for (auto& layer : layers_) {
            // res = (*layer)(res);
            res = layer->operator()(res);
        }
        return res;
    }

    Eigen::MatrixXd backward(Eigen::MatrixXd dx)
    {
        Eigen::MatrixXd dout = dx;

        for (auto rIter = layers_.rbegin(); rIter != layers_.rend(); rIter++) dout = (*rIter)->backward(dout);

        return dout;
    }

    void zeroGrads()
    {
        for (auto& layer : layers_) layer->zeroGrads();
    }

    void updateParams(double learningRate)
    {
        for (auto& layer : layers_) layer->updateParams(learningRate);
    }
};

#endif //! _SEQUENTIAL_HEADER_HPP