add_executable(tcp_test src/tcp_test.cpp)
target_include_directories(tcp_test PUBLIC include)
set_target_properties(tcp_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

add_executable(http_client src/http_client.cpp)
target_include_directories(http_client PUBLIC include)
set_target_properties(http_client PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
target_link_libraries(tcp_test Threads::Threads)
target_link_libraries(http_client Threads::Threads)

find_package(Boost REQUIRED)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(tcp_test ${Boost_LIBRARIES})
	target_link_libraries(http_client ${Boost_LIBRARIES})
endif ()
