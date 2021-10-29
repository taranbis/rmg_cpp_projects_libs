set(SOURCES "./main.cpp")

add_executable(BezierFunction ${SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Modules)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(BezierFunction SDL2::SDL2)
set_target_properties(BezierFunction PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)