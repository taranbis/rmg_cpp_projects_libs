#ifndef _LAYER_HEADER_HPP_
#define _LAYER_HEADER_HPP_ 1
#pragma once

template <typename NumericType>
class Layer
{
public:
    virtual Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> operator()(
                Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> x)
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }

    // need these for loss layers
    virtual NumericType operator()(Eigen::MatrixXd input, Eigen::VectorXd target)
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }

    virtual Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> backward(
                Eigen::Matrix<NumericType, Eigen::Dynamic, Eigen::Dynamic> dx)
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }

    // again, for the loss function
    virtual Eigen::MatrixXd backward()
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }

    virtual void zeroGrads()
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }
    virtual void updateParams(double learningRate)
    {
        throw std::runtime_error("Not implemented. This is an interface");
    }

    virtual ~Layer() = default;
};

#endif //!_LAYER_HEADER_HPP_