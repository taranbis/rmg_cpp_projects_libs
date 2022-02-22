#ifndef _GAME_2048_HEADER_HPP_
#define _GAME_2048_HEADER_HPP_ 1
#pragma once

#include "board_2048.hpp"

enum class Direction { Up, Down, Left, Right };

class Game
{
private:
    uint64_t score{};
    static constexpr std::size_t Rows{4};
    static constexpr std::size_t Columns{4};

public:
    Game(Board<Rows, Columns> board) : board_(board){}

    // bool wonGame()
    // {

    // }

    // double score(){

    // }

    bool canMoveUp() {}

    void move(Direction dir)
    {
        // switch (dir) {
        // case Direction::Up: moveUp();
        // case Direction::Down:
        //     // moveDown();
        // case Direction::Left:
            // moveLeft(board_);
            // case Direction::Right:
            //     // moveRight();
        // }
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

private:
    Board<4, 4> board_;
};


#endif //!_GAME_2048_HEADER_HPP_