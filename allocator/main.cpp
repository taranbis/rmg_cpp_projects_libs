#include <memory_resource>
#include <cstddef>

#include "tracking_allocator.hpp"

int main()
{
    // test speed for now

    std::array<std::byte, 1000>         buffer;
    std::pmr::monotonic_buffer_resource myAllocator{buffer.data(), buffer.size(), std::pmr::new_delete_resource()};

    TrackingAllocator trackingAllocator(&myAllocator, "main", true);

    std::pmr::vector<int> v{&trackingAllocator};

    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
    // trackingAllocator.poll();
    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
    v.emplace_back(3);
	// trackingAllocator.poll();

    return 0;
}