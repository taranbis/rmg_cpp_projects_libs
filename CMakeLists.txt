cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(rmglib VERSION 1.0.0)

if(WIN32)
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".dll")
elseif(UNIX)
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".lib")
endif()

add_subdirectory(vendors/googletest)
target_compile_definitions(gtest PUBLIC GTEST_LANG_CXX20)

include(CTest)
enable_testing()

add_subdirectory(util)
