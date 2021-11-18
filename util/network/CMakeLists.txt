add_executable(tcp_test src/tcp_test.cpp)
target_include_directories(tcp_test PUBLIC include)
set_target_properties(tcp_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
target_link_libraries(tcp_test Threads::Threads)

find_package(Boost REQUIRED)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(tcp_test ${Boost_LIBRARIES})
endif ()