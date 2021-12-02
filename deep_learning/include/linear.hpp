#ifndef _LINEAR_HEADER_HPP_
#define _LINEAR_HEADER_HPP_ 1
#pragma once

#include <Eigen/Dense>

#include "layer.hpp"
#include "util.hpp"

// sensor_input = Eigen::Map<Eigen::MatrixXd>(sensor_input_vector[0].data(),3,sensor_input_vector.size());

template <typename NumericType, size_t batchDim, size_t inDim, size_t outDim>
class Linear : public Layer<NumericType>
{
private:
    template <typename T>
    struct Cache {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> x;
        // Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> w;
        // Eigen::Matrix<T, Eigen::Dynamic, 1> b;
    };
    Cache<NumericType> cache_;

public:
    /**
     * @brief Construct a new Linear object
     *
     * @param bias
     */
    Linear(/* bool bias = true */)
    {
        weights_ = Eigen::Matrix<NumericType, inDim, outDim>::Random();
        bias_ = Eigen::Matrix<NumericType, 1, outDim>::Random();

        zeroGrads();
    }

    Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> operator()(
                Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> x) final override
    {
        // Eigen::Matrix<NumericType, batchDim, inDim> data = x;
        Eigen::MatrixXd data = x;
        cache_.x = x;

        // rowwise() is broadcasting;
        return (x * weights_).rowwise() + bias_;
    }

    void zeroGrads() final override
    {
        dWeights_ = Eigen::Matrix<NumericType, inDim, outDim>::Zero();
        dBias_ = Eigen::Matrix<NumericType, batchDim, inDim>::Zero();
    }

    Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> backward(
                Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> dout) final override
    {
        // std::cout << "------------Backward Linear-------------------" << std::endl;

        dWeights_ = cache_.x.transpose() * dout;
        Eigen::MatrixXd dX = dout * weights_.transpose();
        Eigen::MatrixXd tmp = Eigen::Matrix<NumericType, batchDim, 1>::Ones();
        dBias_ = tmp.transpose() * dout;
        return dX;
    }

    void updateParams(double learningRate) final override
    {
        weights_ -= learningRate * dWeights_;
        bias_ -= learningRate * dBias_;
    }

private:
    // parameters
    Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> weights_;
    Eigen::Matrix<NumericType, 1, Eigen::Dynamic> bias_; // this is a row vector

    // store gradients
    Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> dWeights_;
    Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> dBias_;
};

#endif //!_LINEAR_HEADER_HPP_