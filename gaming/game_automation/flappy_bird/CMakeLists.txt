add_executable(flappy_bird flappy_bird.cpp)
target_include_directories(flappy_bird PUBLIC include)
set_target_properties(flappy_bird PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED )
find_package(X11 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries(flappy_bird Threads::Threads)
target_link_libraries(flappy_bird ${OpenCV_LIBS})
target_link_libraries(flappy_bird X11::X11)
target_include_directories(flappy_bird PUBLIC ../../../util/include)

# if (Boost_FOUND)
# 	include_directories(${Boost_INCLUDE_DIRS})
# 	target_link_libraries(flappy_bird ${Boost_LIBRARIES})
# endif ()