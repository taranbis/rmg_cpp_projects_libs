#ifndef _EIGEN_UTILS_HEADER_HPP_
#define _EIGEN_UTILS_HEADER_HPP_ 1
#pragma once

#include <string>
#include <iostream>

#include <opencv2/core.hpp>

namespace rmg
{
template <typename Derived>
void printSize(const Eigen::MatrixBase<Derived>& b) // EigenBase instead of MatrixBase works as well
{
    std::cout << "size; (rows, cols): " << b.size() << "; (" << b.rows() << ", " << b.cols() << ")" << std::endl;
}
} // namespace rmg

#endif //!_EIGEN_UTILS_HEADER_HPP_
