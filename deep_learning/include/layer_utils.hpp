#ifndef _LAYER_UTILS_HEADER_HPP_
#define _LAYER_UTILS_HEADER_HPP_ 1
#pragma once

#include "../include/util.h"

// TODO:implement this
template <typename NumericType, size_t batchDim, size_t numClasses>
class CrossEntropyLoss : public Layer<NumericType>
{
private:
    // TODO: make this a pointer so we can enbale it as well with requires_grad, by creating it only when
    // needed
    struct Cache {
    };
    Cache cache_;

public:
    NumericType operator()(Eigen::Matrix<NumericType, batchDim, numClasses> input,
                           Eigen::Matrix<NumericType, batchDim, 1> target)
    {
        // return -sum([p[i] * log2(p[i]) for i in range(len(p))])
    }
};

template <typename NumericType, size_t batchDim, size_t numClasses>
class L2Loss : public Layer<NumericType>
{
private:
    // TODO: make this a pointer so we can enbale it as well with requires_grad, by creating it only when
    // needed
    Eigen::MatrixXd dx_;

public:
    NumericType operator()(Eigen::MatrixXd input, Eigen::VectorXd target)
    {
        double loss = personal::computeSquareRoot((input.colwise() - target).squaredNorm());
        dx_ = (input.colwise() - target) / loss;

        return loss;
    }

    Eigen::MatrixXd backward()
    {
        return dx_;
    }
};

// template<typename NumericType, size_t batchDim, size_t dim>
template <typename NumericType>
class ReLU : public Layer<NumericType>
{
private:
    struct CacheRelu {
        Eigen::MatrixXd x;
    };

    CacheRelu cache_;

public:
    Eigen::MatrixXd operator()(Eigen::MatrixXd x) final override
    {
        x = (x.array() < 0).select(0, x);

        cache_.x = x;
        return x;
    }

    Eigen::MatrixXd backward(Eigen::MatrixXd dout) final override
    {
        assert(dout.size() == cache_.x.size());

        Eigen::MatrixXd dx = dout;

        // equivalent to dx[cache_.x <= 0] = 0; from python
        for (size_t i = 0, nRows = dx.rows(), nCols = dx.cols(); i < nCols; ++i) {
            for (size_t j = 0; j < nRows; ++j) {
                if (cache_.x(j, i) <= 0) dx(j, i) = 0;
            }
        }
        return dx;
    }

    // Not needed here
    void zeroGrads() final override {}
    void updateParams(double learningRate) final override {}
};

#endif //!_LAYER_UTILS_HEADER_HPP_