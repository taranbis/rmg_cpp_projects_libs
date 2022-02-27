#include <array>
#include <random>
#include <iostream>
#include <limits>
#include <set>
#include <thread>
#include <chrono>

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>
#include <X11/keysymdef.h>

#include "game_2048.hpp"
#include "board_2048.hpp"

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

class Utils2048
{
public:
    std::array<uint16_t, 65536> rowLeftTable;
    std::array<uint16_t, 65536> rowRightTable;
    std::array<double, 65536> scoreTable;
    std::array<bool, 65536> winTable{false};

    // static let shared = Utils2048()

    Utils2048()
    {
        initializeTables();
    }

    void initializeTables()
    {
        std::puts("Initializing tables...");
        for (uint16_t row = 0; row < 20/* std::numeric_limits<uint16_t>::max() */; row++) {
            std::array<uint16_t, 4> line = {uint16_t((row & 0xF000) >> 12), uint16_t((row & 0xF00) >> 8),
                                       uint16_t((row & 0xF0) >> 4), uint16_t((row & 0xF) >> 0)};

            std::puts("Lines...");
            for (auto entry : line) std::cout << entry << ' ';


            bool contains = false;
            for (auto entry : line) {
                if (entry == 11) contains = true;
            }
            winTable[int(row)] = contains;

            std::puts( (std::string("contains = ") + std::string(BoolToString(contains))).c_str());


            std::vector<uint16_t> scoreValues;
            // scoreValues.reserve(line.size());
            //Back inserter help to do the emplace_back
            std::transform(std::begin(line), std::end(line), std::back_inserter(scoreValues),
                           [](uint16_t v) { return v == 0 ? 0 : ((1 << v) * (v - 1)); });

            // // line.map { v -> UInt in
            // //     let v = UInt(v)
            // //     return v == 0 ? 0 : ((1 << v) * (v - 1))
            // // }


            std::puts("Score values...");
            for (auto entry : scoreValues) std::cout << entry << ' ';
            std::cout << std::endl;
            std::cout << std::endl;

            scoreTable[int(row)] = double(std::reduce(scoreValues.begin(), scoreValues.end()));

            // std::puts("Score Table...");
            // for (auto entry : scoreTable) std::cout << entry << ' ';
            // std::cout << std::endl;
        }
        // for(auto entry : winTable) std::cout << entry << endl;
    }
};

int testDirection()
{
    // Utils2048 test;

    Board<4, 4> board;
    Game game(board);

    game.move(Game::Direction::Left);
    game.move(Game::Direction::Right);
    game.move(Game::Direction::Left);
    game.move(Game::Direction::Right);
    game.move(Game::Direction::Left);

    return 0;
}

int testMirroring()
{
    Board<4, 4> board(std::vector<std::vector<int>>{std::vector<int>{4, 2, 0, 2}, std::vector<int>{2, 0, 0, 2},
                                                    std::vector<int>{4, 0, 2, 0}, std::vector<int>{2, 4, 2, 2}});

    Game game(board);
    game.getBoard().print();
    game.moveRight();

    game.getBoard().print();

    return 0;
}

int testScore()
{
    Board<4, 4> board(std::vector<std::vector<int>>{std::vector<int>{4, 2, 0, 2}, std::vector<int>{2, 0, 0, 2},
                                                    std::vector<int>{4, 0, 2, 0}, std::vector<int>{2, 4, 2, 2}});

    Game game(board);
    game.getBoard().print();
    game.moveRight();

    game.getBoard().print();

    DEB(game.getScore()); // this should pe 12 - write test for it as well
    return 0;
}

int main(){
    
}