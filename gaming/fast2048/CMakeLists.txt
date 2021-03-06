add_executable(fast2048 fast2048.cpp)
target_include_directories(fast2048 PUBLIC include)
set_target_properties(fast2048 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 3.3 REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(X11 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

target_link_libraries(fast2048 Threads::Threads)
target_link_libraries(fast2048 ${OpenCV_LIBS})
target_link_libraries(fast2048 X11::X11)
target_link_libraries(fast2048 X11::Xtst)
include_directories(${EIGEN3_INCLUDE_DIR})

if (Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(fast2048 ${Boost_LIBRARIES})
endif ()
target_include_directories(fast2048 PUBLIC ../../util/include)

add_executable(test2048 test.cpp)
target_include_directories(test2048 PUBLIC include)
set_target_properties(test2048 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(test2048 Threads::Threads)
target_link_libraries(test2048 X11::X11)
target_link_libraries(test2048 X11::Xtst)
target_include_directories(test2048 PUBLIC ../../util/include)

#wanted to use them for key presses. done better with X11
# find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
# target_link_libraries(test2048 Qt5::Core Qt5::Gui)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(test2048 ${Boost_LIBRARIES})
endif ()