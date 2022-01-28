set(SOURCES "main.cpp")

project (allocator)
include_directories(PUBLIC "include" "../util/include" "../vendors/phmap_parallel_hashmap"  "../vendors/google_benchmark")

add_executable(allocator ${SOURCES})
set_target_properties(allocator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

add_executable(benchmark benchmark.cpp)
set_target_properties(benchmark PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
find_package(benchmark REQUIRED)
target_link_libraries(benchmark benchmark::benchmark)