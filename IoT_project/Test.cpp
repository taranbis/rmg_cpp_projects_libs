
#include "Forklift.hpp"
#include "KalmanFilter.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

template <std::size_t S>
using Vector = typename Eigen::Vector<double, S>;

template <std::size_t R, std::size_t C>
using Matrix = typename Eigen::Matrix<double, R, C>;

void MockDataGeneration();

int main()
{
    std::cin.get();
    return 0;
}

void MockDataGeneration()
{
    Forklift<double> forkLift(3423);

    forkLift.MoveToPosition(2, 3);
    forkLift.Wait(5);
    forkLift.MoveToPosition(7, 14);
    forkLift.Wait(5);
    forkLift.MoveToPosition(3, 1);
    forkLift.Wait(5);
    forkLift.MoveToPosition(34, 23);
    forkLift.Wait(5);
    forkLift.MoveToPosition(0, 0);
    forkLift.Wait(5);
}