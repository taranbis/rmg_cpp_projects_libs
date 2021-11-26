set(SOURCES "./draw.cpp")

# GLOB_RECURSE - it works the same as the GLOB with exception that it will traverse
# all subdirectories of the matched directory and will match the files.
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(CFD_Simulation ${SOURCES})
# set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Modules)

include_directories(PUBLIC "../../util/include")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(CFD_Simulation SDL2::SDL2)
set_target_properties(CFD_Simulation PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)