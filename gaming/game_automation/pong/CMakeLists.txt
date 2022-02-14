find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(X11 REQUIRED)

add_executable(pong pong.cpp)
target_link_libraries(pong ${OpenCV_LIBS})
target_link_libraries(pong X11::X11)
