#ifndef _UTILS_2048_HEADER_HPP_
#define _UTILS_2048_HEADER_HPP_ 1
#pragma once

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

    Board() : board_{}
    {
        // board_.setZero();
        // FastRandomGenerator randomGen;
        // board_(randomGen.generate(4), randomGen.generate(4)) = 2;

        FastRandomGenerator randomGen;
        board_[randomGen.generate(Rows * Columns)] = 2;

        printBoard();

        insertRandomTile();
        std::cout << std::boolalpha << "can move left: " << canMoveLeft() << std::endl;
        printBoard();
        insertRandomTile();
        std::cout << std::boolalpha << "can move left: " << canMoveLeft() << std::endl;
        printBoard();
        insertRandomTile();
        std::cout << std::boolalpha << "can move left: " << canMoveLeft() << std::endl;
        printBoard();
        insertRandomTile();
        std::cout << std::boolalpha << "can move left: " << canMoveLeft() << std::endl;
        printBoard();
        insertRandomTile();
        std::cout << std::boolalpha << "can move left: " << canMoveLeft() << std::endl;
        printBoard();
        moveLeft();
        printBoard();
    }

    void printBoard() const
    {
        for (std::size_t row = 0; row < Rows; row++) {
            for (std::size_t col = 0; col < Columns; col++) std::cout << board_[row * Columns + col] << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    std::size_t emptyTiles() const
    {
        std::size_t rv{};
        for (auto& val : board_) {
            if (val == 0) rv++;
        }
        return rv;
    }

    void insertRandomTile()
    {
        if (emptyTiles() == 0) return;

        FastRandomGenerator randomGen;
        bool inserted = false;
        std::size_t insertIdx{};

        while (!inserted) {
            insertIdx = randomGen.generate(Rows * Columns);
            if (board_[insertIdx] == 0) {
                inserted = true;
                board_[insertIdx] = 2;
            }
        }
    }

    bool canMoveLeft() const
    {
        for (std::size_t row = 0; row < Rows; row++) {
            if (board_[row * Columns] == 0) continue;
            for (std::size_t col = 0; col < Columns - 1; col++) {
                if (board_[row * Columns + col + 1] == 0 ||
                    (board_[row * Columns + col] ^ board_[row * Columns + col + 1]) == 0) {
                    break;
                }

                if (col == Columns - 2) return false;
            }
        }
        return true;
    }

    // TODO: move this outside. pass board as an argument -> should be Game
    // TODO: should return the score
    void moveLeft()
    {
        if (!canMoveLeft()) return;

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
            
            //TODO: this is not always
            sparseRow = makeSparseRow(sparseRow);

            auto it = sparseRow.begin();
            for (std::size_t col = 0; col < Columns; col++) {
                if ((it + col) != sparseRow.end())
                    board_[row * Columns + col] = *(it + col);
                else
                    board_[row * Columns + col] = 0;
            }
        }
    }

    std::vector<uint16_t> makeSparseRow(std::size_t row)
    {
        std::vector<uint16_t> rv;
        for (std::size_t col = 0; col < Columns; col++) {
            if (board_[row * Columns + col] != 0) rv.emplace_back(board_[row * Columns + col]);
        }
        return rv;
    }

    std::vector<uint16_t> makeSparseRow(std::vector<uint16_t> row)
    {
        std::vector<uint16_t> rv;
        for (auto it = row.begin(); it != row.end(); ++it) {
            if (*it != 0) rv.emplace_back(*it);
        }
        return rv;
    }

    bool canMoveRight() const
    {
        return canMoveLeft();
    }

private:
    Matrix board_{};
};

enum class Direction { Up, Down, Left, Right };

class Game
{
private:
    uint64_t score{};

public:
    Game(Board<4, 4> board) {}

    // bool wonGame()
    // {

    // }

    // double score(){

    // }

    bool canMoveUp() {}

    void move(Direction dir)
    {
        switch (dir) {
        // case Direction::Up: moveUp();
        // case Direction::Down:
        //     // moveDown();
        case Direction::Left:
            board_.moveLeft();
            // case Direction::Right:
            //     // moveRight();
        }
    }

    // bool gameOver()
    // {
    //     if (wonGame()) return true;

    //     if (!board_.emptyTiles) {
    //         if (canMoveLeft(board_))
    //             return false;
    //         else if (canMoveRight(board_))
    //             return false;
    //         else if (canMoveUp(board_))
    //             return false;
    //         else if (canMoveDowm(board_))
    //             return false;

    //         return true;
    //     }
    //     return false;
    // }

private:
    Board<4, 4> board_;
};

#endif //!_UTILS_2048_HEADER_HPP_