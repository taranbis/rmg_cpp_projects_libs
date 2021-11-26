set(SOURCES "main.cpp")

project (allocator)

include_directories(PUBLIC "include" "../util/include" "../vendors/phmap_parallel_hashmap")
add_executable(allocator ${SOURCES})
set_target_properties(allocator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

add_executable(test_allocator test.cpp)
set_target_properties(test_allocator PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
