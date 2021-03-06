#include <memory_resource>
#include <cstddef>

#include "profiler.hpp"
#include "util.hpp"

#include "tracking_allocator.hpp"
#include "memory_pool_allocator.hpp"

struct Test16 {
    double d1_, d2_;
    // char c1, c2, c3;
    Test16(double d1, double d2) : d1_(d1), d2_(d2) {}
};

int main()
{
    constexpr size_t iterations = 1000;
    constexpr size_t size = 1000;

    // DEB_SHORT(sizeof (Test16));
    // NEWLINE();

    TrackingAllocator defaultResourceTracker(std::pmr::new_delete_resource(), "Default Allocator", true);
    std::pmr::set_default_resource(&defaultResourceTracker);

    // TrackingAllocator def(std::pmr::get_default_resource(), "Default Resource", true);
    std::array<std::byte, 10000> raw;
    std::pmr::monotonic_buffer_resource bufferMemSource{raw.data(), raw.size()};
    TrackingAllocator monotonicBufferResourceTracker(&bufferMemSource, "Monotonic Buffer Resource", true);
    // MemoryPool memPool(&bufferMemSource, 16, 100);
    // MemoryPool memPool(&monotonicBufferResourceTracker, 32, 1000);

    std::pmr::pool_options opt;
    opt.largest_required_pool_block = 32;
    opt.max_blocks_per_chunk = 1000;
    std::pmr::unsynchronized_pool_resource memPool(opt, &monotonicBufferResourceTracker);
    TrackingAllocator memPoolTrackingAllocator(&memPool, "Memory Pool Allocator", false);

    std::pmr::list<Test16> test16ListMyPMRPoolAllocator{&memPoolTrackingAllocator};

    for (std::size_t j = 0; j < iterations; ++j) {
        test16ListMyPMRPoolAllocator.clear();
        for (size_t i = 0; i < size; ++i) test16ListMyPMRPoolAllocator.emplace_back(i, i);
    }

    return 0;
}