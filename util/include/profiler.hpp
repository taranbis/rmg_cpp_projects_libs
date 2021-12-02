#ifndef _PROF_HEADER_HPP_
#define _PROF_HEADER_HPP_ 1
#pragma once

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>

#include "parallel_hashmap/phmap.h"
#include "spinlock.hpp"

#include "util.hpp"

/********************************************************************************
 *! Header file that defines the profiling class ProfTimer used to benchmark code
 *! on the principle of RAII along with other useful commands used for debugging
 *! and profiling
 *******************************************************************************/

#define PROF_START_BLOCK(name) \
    {                          \
        ProfTimer ANONYMOUS_NAME(name);
#define PROF_END_BLOCK }

#define PROF_FUNCTION()          \
    std::string func = __func__; \
    ProfTimer ANONYMOUS_NAME(func);

#define PROF_FUNCTION_W_NAME(name) ProfTimer ANONYMOUS_NAME(name);

class ProfBase
{
public:
    static ProfBase& getInstance()
    {
        static ProfBase prof;
        return prof;
    }

    static void Print()
    {
        double totalTime = 0;
        for (const auto& x : ProfBase::getInstance().timeTable_) { totalTime += x.second; }
        std::cout << "====----------- Profiling Information: -----------====" << std::endl;
        for (auto& x : ProfBase::getInstance().timeTable_) {
            double percentage = x.second / totalTime * 100;
            double millis = x.second;
            double tmp = x.second / 1000;
            int mins = tmp / 60;
            double seconds = tmp - mins * 60;
            printf("%25s = %15.3f ms: %4d min, %4.2f s, %6.3f %% \n", x.first.c_str(), millis, mins, seconds,
                   percentage);
        }
    }

private:
    ProfBase() = default;
    ~ProfBase() = default;

public:
    phmap::flat_hash_map<std::string, double> timeTable_;
    rmg::spinlock timeTableSpinlock_;
};

class ProfTimer
{
public:
    ProfTimer(std::string name) : name_(name)
    {
        startTimePoint_ = std::chrono::high_resolution_clock::now();
    }
    ~ProfTimer()
    {
        Stop();
    }

    void Stop()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint_)
                                 .time_since_epoch()
                                 .count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint)
                               .time_since_epoch()
                               .count();
        double duration = end - start;
        std::lock_guard<rmg::spinlock> lg(ProfBase::getInstance().timeTableSpinlock_);
        ProfBase::getInstance().timeTable_[name_] += duration * 0.001;
    }

private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint_;

    // Normal std mutexes and hashmaps
    // static std::unordered_map<std::string, double>              TimeTable;
    // static std::mutex                                           TimeTableMutex;
};

#endif //! _PROF_HEADER_HPP_
