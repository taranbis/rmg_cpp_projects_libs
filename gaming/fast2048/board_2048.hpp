#ifndef _BOARD_2048_HEADER_HPP_
#define _BOARD_2048_HEADER_HPP_ 1
#pragma once

#include <random>

// #include <Eigen/Dense>

class FastRandomGenerator
{
public:
    FastRandomGenerator(uint32_t seed = 0)
    {
        if (seed != 0) {
            seed_ = seed;
            return;
        }
        std::random_device rd;
        seed_ = rd();
    }

    uint32_t nextRandom() noexcept
    {
        seed_ ^= (seed_ << 13);
        seed_ ^= (seed_ >> 17);
        seed_ ^= (seed_ << 5);
        return seed_;
    }

    uint32_t generate(uint32_t range) noexcept
    {
        return uint32_t((uint64_t(nextRandom()) * uint64_t(range)) >> 32);
    }

private:
    uint32_t seed_;
};

template <std::size_t Rows, std::size_t Columns>
class Board
{
public:
    // using Row = typename Eigen::Matrix<uint16_t, Rows, 1>;
    // using Matrix = typename Eigen::Matrix<uint16_t, Rows, Columns>;

    using Row = std::array<uint16_t, Rows>;
    using Matrix = std::array<uint16_t, Rows * Columns>;
    using xyCoordinate = std::pair<uint16_t, uint16_t>;
    using xyCoordinates = std::vector<xyCoordinate>;

    bool operator<=>(const Board& other) const = default;

    Board() = default;
    Board(const Board& other) = default;
    Board& operator=(const Board& other) = default;

    Board(const std::vector<std::vector<int>>& values)
    {
        if (values.size() > Rows) return;

        for (size_t row = 0; row < values.size(); ++row) {
            for (size_t col = 0; col < values[row].size(); ++col) {
                if (values[row].size() > Columns) return;
                board_[row * Columns + col] = values[row][col];
            }
        }
    }

    uint16_t& operator[](std::size_t pos)
    { // No bounds checking performed;
        return board_[pos];
    }

    const uint16_t& operator[](std::size_t pos) const
    { // No bounds checking performed;
        return board_[pos];
    }

    uint16_t& at(std::size_t x, std::size_t y)
    { // No bounds checking performed;
        return board_[x * Columns + y];
    }

    void setZero()
    {
        for (std::size_t row = 0; row < Rows; row++) {
            for (std::size_t col = 0; col < Columns; col++) board_[row * Columns + col] = 0;
        }
    }

    std::size_t countEmptyTiles() const
    {
        std::size_t rv{};
        for (auto& val : board_) {
            if (val == 0) rv++;
        }
        return rv;
    }
    
    void print() const
    {
        std::cout << "-------------------Board-------------------------------" << std::endl;
        for (std::size_t row = 0; row < Rows; row++) {
            for (std::size_t col = 0; col < Columns; col++) std::cout << board_[row * Columns + col] << " ";
            std::cout << std::endl;
        }
        std::cout << "-------------------------------------------------------" << std::endl;
    }

private:
    Matrix board_{};
};

template <std::size_t Rows, std::size_t Columns>
std::ostream& operator<<(std::ostream& os, const Board<Rows, Columns> board)
{
    os << std::endl;
    for (std::size_t row = 0; row < Rows; row++) {
        for (std::size_t col = 0; col < Columns; col++) os << board[row * Columns + col] << " ";
        os << std::endl;
    }
    return os;
}

#endif //!_BOARD_2048_HEADER_HPP_