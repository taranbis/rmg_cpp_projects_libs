#include <memory_resource>
#include <cstddef>

#include <benchmark/benchmark.h>

#include "profiler.hpp"
#include "util.hpp"

#include "tracking_allocator.hpp"
#include "segregator.hpp"
#include "memory_pool_allocator.hpp"

struct Test16 {
    double d1_, d2_;
    // char c1, c2, c3;
    Test16(double d1, double d2) : d1_(d1), d2_(d2) {}
};

struct Test8 {
    double d1_;
    // char c1, c2, c3;
    Test8(double d1) : d1_(d1) {}
};
struct Test4 {
    int d1_;
    // char c1, c2, c3;
    Test4(int d1) : d1_(d1) {}
};
struct Test2 {
    short d1_;
    // char c1, c2, c3;
    Test2(short d1) : d1_(d1) {}
};

// static size_t numRepetitions = 100;
static size_t size = 100;

static void BM_PMRListComplexAllocator(benchmark::State& state)
{
    // TrackingAllocator def(std::pmr::get_default_resource(), "Default Resource", true);

    std::array<std::byte, 32000> raw;
    std::pmr::monotonic_buffer_resource bufferMemSource{raw.data(), raw.size(), std::pmr::get_default_resource()};

    // TrackingAllocator monotonicBufferResourceTracker(&bufferMemSource, "Monotonic Buffer Resource", true);

    std::pmr::pool_options opt;
    opt.largest_required_pool_block = 16;
    opt.max_blocks_per_chunk = 100;
    std::pmr::unsynchronized_pool_resource pool(opt, &bufferMemSource);

    std::pmr::list<Test16> test16ListPMR{&pool};
    std::pmr::list<Test8> test8ListPMR{&pool};
    std::pmr::list<Test4> test4ListPMR{&pool};
    std::pmr::list<Test2> test2ListPMR{&pool};
    for (auto _ : state) {
        // This code gets timed
        test16ListPMR.clear();
        test8ListPMR.clear();
        test4ListPMR.clear();
        test2ListPMR.clear();
        for (size_t i = 0; i < size; ++i) test16ListPMR.emplace_back(i, i);
        for (size_t i = 0; i < size; ++i) test8ListPMR.emplace_back(i);
        for (size_t i = 0; i < size; ++i) test4ListPMR.emplace_back(i);
        for (size_t i = 0; i < size; ++i) test2ListPMR.emplace_back(i);
        benchmark::DoNotOptimize(test16ListPMR);
        benchmark::DoNotOptimize(test8ListPMR);
        benchmark::DoNotOptimize(test4ListPMR);
        benchmark::DoNotOptimize(test2ListPMR);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_PMRListComplexAllocator);

static void BM_STDListComplexAllocator(benchmark::State& state)
{
    std::list<Test16> test16List{};
    std::list<Test8> test8List{};
    std::list<Test4> test4List{};
    std::list<Test2> test2List{};
    for (auto _ : state) {
        // This code gets timed
        test16List.clear();
        test8List.clear();
        test4List.clear();
        test2List.clear();
        for (size_t i = 0; i < size; ++i) test16List.emplace_back(i, i);
        for (size_t i = 0; i < size; ++i) test8List.emplace_back(i);
        for (size_t i = 0; i < size; ++i) test4List.emplace_back(i);
        for (size_t i = 0; i < size; ++i) test2List.emplace_back(i);
        benchmark::DoNotOptimize(test16List);
        benchmark::DoNotOptimize(test8List);
        benchmark::DoNotOptimize(test4List);
        benchmark::DoNotOptimize(test2List);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_STDListComplexAllocator);

static void BM_PMRListSimple(benchmark::State& state)
{
    for (auto _ : state) {
        std::byte raw[2048];
        std::pmr::monotonic_buffer_resource bufferMemSource{raw, sizeof raw, std::pmr::get_default_resource()};
        std::pmr::list<int> listOfThings{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, &bufferMemSource};
        benchmark::DoNotOptimize(listOfThings);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_PMRListSimple);

static void BM_STDListSimple(benchmark::State& state)
{
    for (auto _ : state) {
        std::list<int> listOfThings{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
        benchmark::DoNotOptimize(listOfThings);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_STDListSimple);

static void BM_PMRTest16ListComplexAllocator(benchmark::State& state)
{
    // TrackingAllocator def(std::pmr::get_default_resource(), "Default Resource", true);

    std::array<std::byte, 32000> raw;
    std::pmr::monotonic_buffer_resource bufferMemSource{raw.data(), raw.size(), std::pmr::get_default_resource()};

    // TrackingAllocator monotonicBufferResourceTracker(&bufferMemSource, "Monotonic Buffer Resource", true);

    std::pmr::pool_options opt;
    opt.largest_required_pool_block = 32;
    opt.max_blocks_per_chunk = 1000;
    std::pmr::unsynchronized_pool_resource pool(opt, &bufferMemSource);

    std::pmr::list<Test16> test16ListPMR{&pool};
    for (auto _ : state) {
        // This code gets timed
        // test16ListPMR.clear();
        for (size_t i = 0; i < size; ++i) test16ListPMR.emplace_back(i, i);
        benchmark::DoNotOptimize(test16ListPMR);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_PMRTest16ListComplexAllocator);

static void BM_STDTest16ListComplexAllocator(benchmark::State& state)
{
    std::list<Test16> test16List{};
    for (auto _ : state) {
        // This code gets timed
        // test16List.clear();
        for (size_t i = 0; i < size; ++i) test16List.emplace_back(i, i);
        benchmark::DoNotOptimize(test16List);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_STDTest16ListComplexAllocator);

static void BM_MyPMRAllocatorTest16ListComplexAllocator(benchmark::State& state)
{
    // TrackingAllocator def(std::pmr::get_default_resource(), "Default Resource", true);
    std::array<std::byte, 32000> raw;
    std::pmr::monotonic_buffer_resource bufferMemSource{raw.data(), raw.size(), std::pmr::get_default_resource()};

    // TrackingAllocator monotonicBufferResourceTracker(&bufferMemSource, "Monotonic Buffer Resource", true);
    // MemoryPool memPool(&bufferMemSource, 16, 100);
    MemoryPool memPool(&bufferMemSource, 32, 1000);

    std::pmr::list<Test16> test16ListMyPMRPoolAllocator{&memPool};
    for (auto _ : state) {
        // This code gets timed
        // test16ListMyPMRPoolAllocator.clear();
        for (size_t i = 0; i < size; ++i) test16ListMyPMRPoolAllocator.emplace_back(i, i);
        benchmark::DoNotOptimize(test16ListMyPMRPoolAllocator);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_MyPMRAllocatorTest16ListComplexAllocator);

// Run the benchmark
BENCHMARK_MAIN();