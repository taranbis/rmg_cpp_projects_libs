#ifndef DATA_UTILS_HEADER_HPP
#define DATA_UTILS_HEADER_HPP 1
#pragma once

#include<stddef.h>
#include <fstream>
#include <iostream>

#include <Eigen/Dense>

#include "util.hpp"
#include <cstdio>


template<std::size_t batchDim>
std::pair<Eigen::MatrixXd, Eigen::MatrixXd> loadBatchCIFAR10(const char *fileName)
{
    std::fstream file;
    file.open(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.good()) std::cerr << "File not found: " << fileName << std::endl;
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Each file contains 10000 such 3073-byte "rows" of images, although there is nothing delimiting the
    // rows. Therefore each file should be exactly 30730000 bytes long.

    // first byte is label of the image
    uint8_t label;

    // next 3072 bytes are the values of the pixels of the image.
    // first 1024 bytes are the red channel values, the next 1024 the green, and the final 1024 the blue
    // The values are stored in row-major order, so the first 32 bytes are the red channel values of the first
    // row of the image.
    uint8_t rChannel[1024];
    uint8_t gChannel[1024];
    uint8_t bChannel[1024];

    // constexpr size_t batchDim = 10000;
    constexpr size_t imgByteLength = 3072;
    Eigen::MatrixXd rv(batchDim, imgByteLength);
    Eigen::MatrixXd labels = Eigen::Matrix<double, batchDim, 1>::Zero();

    // Each file contains 10000 such 3073-byte "rows" of images
    for (std::size_t i = 0; i < batchDim; ++i) {
        if (!file.read((char *)&label, sizeof(label))) std::cerr << "Error reading label" << std::endl;
        if (!file.read((char *)&rChannel, 1024)) std::cerr << "Error red channel label" << std::endl;
        if (!file.read((char *)&gChannel, 1024)) std::cerr << "Error green channel label" << std::endl;
        if (!file.read((char *)&bChannel, 1024)) std::cerr << "Error blue channel label" << std::endl;

        labels(i, 0) = label;
        for (std::size_t j = 0; j < imgByteLength / 3; ++j) {
            rv(i, 3 * j) = rChannel[j];
            rv(i, 3 * j + 1) = gChannel[j];
            rv(i, 3 * j + 2) = bChannel[j];
        }
    }

    return {rv, labels};
}

void loadCIFAR10();

void getDataCIFAR10(size_t numTraining, size_t numValidation, size_t numTest);

double scoreFunction(double x, double linExpBoundary, double doublingRate);

#endif //!DATA_UTILS_HEADER_HPP