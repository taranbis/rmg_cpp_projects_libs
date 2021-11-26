#ifndef _OPTIM_HEADER_HPP_
#define _OPTIM_HEADER_HPP_ 1
#pragma once

/**
 * @brief Performs vanilla stochastic gradient descent.
 */
void sgd(Eigen::MatrixXd &w, const Eigen::MatrixXd &dw, double learningRate = 1e-3)
{
    w -= learningRate * dw;
}

//TODO: implement interface Optimizer;implement classes Adam and SGD

#endif //!_OPTIM_HEADER_HPP_