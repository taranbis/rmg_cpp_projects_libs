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

        for (size_t i = 0; i < values.size(); ++i) {
            for (size_t j = 0; j < values[i].size(); ++j) {
                if (values[i].size() > Columns) return;
                board_[i * Columns + j] = values[i][j];
            }
        }
    }

    uint16_t& operator[](std::size_t pos)
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

    // TODO: should be a function outside of the class
    void printBoard() const
    {
        std::cout << "-------------------Board-------------------------------" << std::endl;
        for (std::size_t row = 0; row < Rows; row++) {
            for (std::size_t col = 0; col < Columns; col++) std::cout << board_[row * Columns + col] << " ";
            std::cout << std::endl;
        }
        std::cout << "-------------------------------------------------------" << std::endl;
    }

    std::size_t countEmptyTiles() const
    {
        std::size_t rv{};
        for (auto& val : board_) {
            if (val == 0) rv++;
        }
        return rv;
    }

    bool canMoveLeft()
    {
        // bool rv = true;
        // for (std::size_t row = 0; row < Rows; row++) {
        //     bool rowCanMove = true;
        //     if (board_[row * Columns] == 0) {
        //         rowCanMove &= true;
        //         // return true;
        //         continue;
        //     }
        //     for (std::size_t col = 1; col < Columns - 1; col++) {
        //         if ((board_[row * Columns + col - 1] ^ board_[row * Columns + col]) == 0) {
        //             rowCanMove &= true;
        //             // return true;
        //             break;
        //         }

        //         if(board_[row * Columns + col] == 0){}

        //         if (col == Columns - 2) rv &= false;
        //     }
        // }
        // // return rv;
        // return false;

        Board<Rows, Columns> board(*this);
        moveLeft(board);
        if (board_ == board.board_) return false;
        return true;
    }

    // TODO: move this outside. pass board as an argument -> should be Game
    // TODO: should return the score
    void moveLeft(Board<Rows, Columns>& board)
    {
        // if (!canMoveLeft()) return;

        for (std::size_t row = 0; row < Rows; row++) {
            std::vector<uint16_t> sparseRow = makeSparseRow(row);

            std::puts("sparse row");
            for (auto& val : sparseRow) std::cout << val << " ";
            std::cout << std::endl;

            for (auto it = sparseRow.begin() + 1; it != sparseRow.end() && sparseRow.size() >= 2; ++it) {
                if (*it == 0) continue;
                if (*it == *(it - 1)) {
                    *(it - 1) = *it + *(it - 1);
                    *it = 0;
                }
            }

            // TODO: this is not always
            sparseRow = makeSparseRow(sparseRow);

            auto it = sparseRow.cbegin();
            for (std::size_t col = 0; col < Columns; col++) {
                if ((it + col) < sparseRow.cend()) {
                    //! != was exactly that. i needed to have < iter to work
                    board[row * Columns + col] = *(it + col);
                } else
                    board[row * Columns + col] = 0;
            }
        }
    }

    std::vector<uint16_t> makeSparseRow(std::size_t row) const
    {
        std::vector<uint16_t> rv{};
        for (std::size_t col = 0; col < Columns; col++) {
            if (board_[row * Columns + col] != 0) rv.emplace_back(board_[row * Columns + col]);
        }
        return rv;
    }

    std::vector<uint16_t> makeSparseRow(const std::vector<uint16_t>& row) const
    {
        std::vector<uint16_t> rv{};
        for (auto it = row.cbegin(); it != row.cend(); ++it) {
            if (*it != 0) rv.emplace_back(*it);
        }
        return rv;
    }

    bool canMoveRight() const
    {
        // TODO_2048 NOT TRUE
        return canMoveLeft();
    }

private:
    Matrix board_{};
};

#endif //!_BOARD_2048_HEADER_HPP_