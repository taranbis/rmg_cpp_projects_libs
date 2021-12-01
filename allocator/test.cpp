#include <memory_resource>
#include <cstddef>

#include "tracking_allocator.hpp"
#include "segregator.hpp"

#include "profiler.hpp"
#include "util.hpp"

struct Test {
public:
    double d1, d2;
    // char c1, c2, c3;
};

int main()
{


    std::pmr::pool_options opt;

    opt.largest_required_pool_block = 1024;
    opt.max_blocks_per_chunk = 8;
    std::pmr::unsynchronized_pool_resource pool1024(opt, std::pmr::get_default_resource());

    opt.largest_required_pool_block = 512;
    opt.max_blocks_per_chunk = 16;
    std::pmr::unsynchronized_pool_resource pool512(opt, &pool1024);

    opt.largest_required_pool_block = 256;
    opt.max_blocks_per_chunk = 32;
    std::pmr::unsynchronized_pool_resource pool256(opt, &pool512);

    opt.largest_required_pool_block = 128;
    opt.max_blocks_per_chunk = 64;
    std::pmr::unsynchronized_pool_resource pool128(opt, &pool256);

    opt.largest_required_pool_block = 64;
    opt.max_blocks_per_chunk = 256;
    std::pmr::unsynchronized_pool_resource pool64(opt, &pool128);

    opt.largest_required_pool_block = 32;
    opt.max_blocks_per_chunk = 512;
    std::pmr::unsynchronized_pool_resource pool32(opt, &pool64);

    opt.largest_required_pool_block = 16;
    opt.max_blocks_per_chunk = 1024;
    std::pmr::unsynchronized_pool_resource pool16(opt, &pool32);

    opt.largest_required_pool_block = 8;
    opt.max_blocks_per_chunk = 2056;
    std::pmr::unsynchronized_pool_resource pool8(opt, &pool16);

    opt.largest_required_pool_block = 4;
    opt.max_blocks_per_chunk = 1024;
    std::pmr::unsynchronized_pool_resource pool4(opt, &pool8);

    opt.largest_required_pool_block = 2;
    opt.max_blocks_per_chunk = 128;
    std::pmr::unsynchronized_pool_resource pool2(opt, &pool4);

    opt.largest_required_pool_block = 1;
    opt.max_blocks_per_chunk = 128;
    std::pmr::unsynchronized_pool_resource pool1(opt, &pool2);

    opt.largest_required_pool_block = 16;
    opt.max_blocks_per_chunk = 50000;
    std::pmr::unsynchronized_pool_resource pool16_TEST(opt, std::pmr::get_default_resource());

    // std::array<std::byte, 50000 * 16> raw;
    // std::pmr::monotonic_buffer_resource bufferMemSource{raw.data(), raw.size(), std::pmr::get_default_resource()};

    constexpr size_t numRepetitions = 5;
    constexpr size_t size = 50000;
    std::cout << "Speed in allocating " << size << " objects of size: " << sizeof(Test) << std::endl;

    PROF_START_BLOCK("My Allocator");
    std::pmr::list<Test> testListPMR{&pool16_TEST};
    for (size_t j = 0; j < numRepetitions; j++) {
        testListPMR.clear();
        for (size_t i = 0; i < size; ++i) testListPMR.insert(testListPMR.end(), Test{});
    }
    PROF_END_BLOCK;

    ProfBase::Print();

    PROF_START_BLOCK("Default allocator");
    std::list<Test> testList{};
    for (size_t j = 0; j < numRepetitions; j++) {
        testList.clear();
        for (size_t i = 0; i < size; ++i) testList.insert(testList.end(), Test{});
    }
    PROF_END_BLOCK;

    ProfBase::Print();
    return 0;
}

int main2()
{
    std::pmr::pool_options opt;

    TrackingAllocator def(std::pmr::get_default_resource(), "default", true);
    opt.largest_required_pool_block = 32;
    opt.max_blocks_per_chunk = 512;
    std::pmr::synchronized_pool_resource pool32(opt, std::pmr::get_default_resource());
    TrackingAllocator trackPool32(&pool32, "pool32", true);

    opt.largest_required_pool_block = 16;
    opt.max_blocks_per_chunk = 1024;
    std::pmr::synchronized_pool_resource pool16(opt, &pool32);
    TrackingAllocator trackPool16(&pool16, "pool16", true);

    opt.largest_required_pool_block = 8;
    opt.max_blocks_per_chunk = 2056;
    std::pmr::synchronized_pool_resource pool8(opt, &pool16);
    TrackingAllocator trackPool8(&pool8, "pool8", true);

    opt.largest_required_pool_block = 4;
    opt.max_blocks_per_chunk = 1024;
    std::pmr::synchronized_pool_resource pool4(opt, &pool8);
    TrackingAllocator trackPool4(&pool4, "pool4", true);

    opt.largest_required_pool_block = 2;
    opt.max_blocks_per_chunk = 128;
    std::pmr::synchronized_pool_resource pool2(opt, &pool4);
    TrackingAllocator trackPool2(&pool2, "pool2", true);

    opt.largest_required_pool_block = 1;
    opt.max_blocks_per_chunk = 128;
    std::pmr::synchronized_pool_resource pool1(opt, &pool2);
    TrackingAllocator trackPool(&pool1, "pool1", true);

    std::vector<int, std::pmr::polymorphic_allocator<int>> v{&pool1};

    // v.reserve(20);

    v.emplace_back(2);
    // v.emplace_back(3);
    // v.emplace_back(4);

    return 0;
}