#ifndef _GAME_2048_HEADER_HPP_
#define _GAME_2048_HEADER_HPP_ 1
#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>
#include <X11/keysymdef.h>

#include "util.hpp"
#include "board_2048.hpp"

class Game
{
private:
    uint64_t score{};
    static constexpr std::size_t Rows{4};
    static constexpr std::size_t Columns{4};

public:
    enum class Direction { Left, Right, Up, Down };

    Game(Board<Rows, Columns> board) : board_(board)
    {
        display_ = XOpenDisplay(nullptr);
    }

    ~Game()
    {
        XCloseDisplay(display_);
    }

    // bool wonGame()
    // {

    // }

    // bool canMoveUp() {}

    void move(Direction dir) const
    {
        KeySym keyCode; 
        switch (dir) {
        case Direction::Up: 
            keyCode = XK_Up;
            break;
        case Direction::Down:
            keyCode = XK_Down;
            break;
        case Direction::Left:
            keyCode = XK_Left;
            break;
        case Direction::Right:
            keyCode = XK_Right;
            break;
        // default:
        //     keyCode = XK_VoidSymbol;
        }

        doMove(keyCode);
    }

    void insertRandomTile(Board<Rows, Columns> & board)
    {
        if (board.countEmptyTiles() == 0) return;

        FastRandomGenerator randomGen;
        bool inserted = false;
        std::size_t insertIdx{};

        while (!inserted) {
            insertIdx = randomGen.generate(Rows * Columns);
            if (board[insertIdx] == 0) {
                inserted = true;
                board[insertIdx] = 2;
            }
        }
    }

    std::vector<uint16_t> makeSparseRow(const Board<Rows, Columns>& board, std::size_t row) const
    {
        std::vector<uint16_t> rv{};
        for (std::size_t col = 0; col < Columns; col++) {
            if (board[row * Columns + col] != 0) rv.emplace_back(board[row * Columns + col]);
        }
        return rv;
    }

    std::vector<uint16_t> makeSparseColumn(const Board<Rows, Columns>& board, std::size_t col) const
    {
        std::vector<uint16_t> rv{};
        for (std::size_t row = 0; row < Rows; row++) {
            if (board[row * Columns + col] != 0) rv.emplace_back(board[row * Columns + col]);
        }
        return rv;
    }

    std::vector<uint16_t> makeSparseVector(const std::vector<uint16_t>& row) const
    {
        std::vector<uint16_t> rv{};
        for (auto it = row.cbegin(); it != row.cend(); ++it) {
            if (*it != 0) rv.emplace_back(*it);
        }
        return rv;
    }

    Board<Rows, Columns> mirrorBoardOnYAxes(const Board<Rows, Columns>& board) const
    {
        Board<Rows, Columns> rv{}; 
        for (std::size_t row = 0; row < Rows; row++) {
            for (std::size_t col = 0; col < Columns; col++) { 
                rv.at(row, Columns - 1 - col) = board[row * Columns + col];
            }
        }
        return rv;
    }

    Board<Rows, Columns> mirrorBoardOnXAxes(const Board<Rows, Columns>& board) const
    {
        Board<Rows, Columns> rv{};
        for (std::size_t col = 0; col < Columns; col++) {
            for (std::size_t row = 0; row < Rows; row++) {
                rv.at(Rows - 1 - row, col) = board[row * Columns + col];
            }
        }
        return rv;
    }

    bool canMoveLeft()
    {
        Board<Rows, Columns> board(board_);
        doMoveLeft(board);
        if (board_ == board) return false;
        return true;
    }

    void moveLeft()
    {
        score_ += doMoveLeft(board_);
    }

    bool canMoveRight()
    {
        Board<Rows, Columns> board(board_);
        doMoveRight(board);
        if (board_ == board) return false;
        return true;
    }

    void moveRight()
    {
        score_ += doMoveRight(board_);
    }

    bool canMoveUp()
    {
        Board<Rows, Columns> board(board_);
        doMoveUp(board);
        if (board_ == board) return false;
        return true;
    }

    void moveUp()
    {
        score_ += doMoveUp(board_);
    }

    bool canMoveDown()
    {
        Board<Rows, Columns> board(board_);
        doMoveDown(board);
        if (board_ == board) return false;
        return true;
    }

    void moveDown()
    {
        score_ += doMoveDown(board_);
    }

    Board<Rows, Columns> getBoard() const
    {
        return board_;
    }

    void setBoard(const Board<Rows, Columns>& board)
    {
        board_ = board;
    }

    //should be a function outside of the board class
    // bool gameOver()
    // {
    //     if (wonGame()) return true;

    //     if (!board_.countEmptyTiles) {
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

    Direction findBestMove()
    {
        Direction rv{Direction::Left};
        std::size_t maxScore = 0;

        // auto callFunctionAndComputeScore = [&maxScore, &rv, this](std::function<std::size_t(Board<Rows,
        // Columns>)> fn) {
        //     const std::size_t score = fn(Board<Rows, Columns>(board_));
        //     if (score > maxScore) maxScore = score;
        // };

        // Up
        if (canMoveUp()) {
            // auto callMoveRight = [this](Board<Rows, Columns> board) { return doMoveRight(board); };
            // callFunctionAndComputeScore(callMoveRight);
            Board<Rows, Columns> board(board_);
            const std::size_t score = doMoveUp(board);
            if (score >= maxScore) {
                maxScore = score;
                rv = Direction::Up;
            }
        }

        // Right
        if (canMoveRight()) {
            Board<Rows, Columns> board(board_);
            const std::size_t score = doMoveRight(board);
            if (score >= maxScore) {
                maxScore = score;
                rv = Direction::Right;
            }
        }

        // Left
        if (canMoveLeft()) {
            Board<Rows, Columns> board(board_);
            const std::size_t score = doMoveLeft(board);
            if (score >= maxScore) {
                maxScore = score;
                rv = Direction::Left;
            }
        }

        // Down
        if (canMoveDown()) {
            Board<Rows, Columns> board(board_);
            const std::size_t score = doMoveDown(board);
            if (score >= maxScore) {
                maxScore = score;
                rv = Direction::Down;
            }
        }

        return rv;
    }

    std::size_t getScore() const
    {
        return score_;
    }

private:
    void doMove(KeySym keyCode) const
    {
        // Press key code
        KeyCode modcode = XKeysymToKeycode(display_, keyCode);
        XTestFakeKeyEvent(display_, modcode, False, 0);
        XFlush(display_);
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        XTestFakeKeyEvent(display_, modcode, True, 0);
        XFlush(display_);
        XTestFakeKeyEvent(display_, modcode, False, 0);
        XFlush(display_);
    }

    std::size_t doMoveLeft(Board<Rows, Columns>& board)
    {
        std::size_t score = 0;
        for (std::size_t row = 0; row < Rows; row++) {
            std::vector<uint16_t> sparseRow = makeSparseRow(board, row);

            for (auto it = sparseRow.begin() + 1; it != sparseRow.end() && sparseRow.size() >= 2; ++it) {
                if (*it == 0) continue;
                if (*it == *(it - 1)) {
                    *(it - 1) += *it;
                    score += *(it - 1);
                    *it = 0;
                }
            }

            sparseRow = makeSparseVector(sparseRow);

            auto it = sparseRow.cbegin();
            for (std::size_t col = 0; col < Columns; col++) {
                if ((it + col) < sparseRow.cend()) {
                    //! != was exactly that. i needed to have < iter to work.otherwise the iterator was going forward
                    board[row * Columns + col] = *(it + col);
                } else
                    board[row * Columns + col] = 0;
            }
        }
        return score;
    }

    std::size_t doMoveUp(Board<Rows, Columns>& board)
    {
        std::size_t score = 0;
        for (std::size_t col = 0; col < Columns; col++) {
            std::vector<uint16_t> sparseColumn = makeSparseColumn(board, col);

            // std::puts("sparse column");
            // for(auto x : sparseColumn) std::cout<<x << " ";
            // std::cout << std::endl;

            for (auto it = sparseColumn.begin() + 1; it != sparseColumn.end() && sparseColumn.size() >= 2; ++it) {
                if (*it == 0) continue;
                if (*it == *(it - 1)) {
                    *(it - 1) += *it;
                    score += *(it - 1);
                    *it = 0;
                }
            }

            sparseColumn = makeSparseVector(sparseColumn);

            auto it = sparseColumn.cbegin();
            for (std::size_t row = 0; row < Rows; row++) {
                if ((it + row) < sparseColumn.cend()) {
                    //! != was exactly that. i needed to have < iter to work.otherwise the iterator was going forward
                    board[row * Columns + col] = *(it + row);
                } else
                    board[row * Columns + col] = 0;
            }
        }
        return score;
    }

    std::size_t doMoveRight(Board<Rows, Columns>& board)
    {
        board = mirrorBoardOnYAxes(board);
        const std::size_t score = doMoveLeft(board);
        board = mirrorBoardOnYAxes(board);
        return score;
    }


    std::size_t doMoveDown(Board<Rows, Columns>& board)
    {
        board = mirrorBoardOnXAxes(board);
        const std::size_t score = doMoveUp(board);
        board = mirrorBoardOnXAxes(board);
        return score;
    }


private:
    Board<Rows, Columns> board_;
    Display* display_;
    std::size_t score_{};
};


#endif //!_GAME_2048_HEADER_HPP_