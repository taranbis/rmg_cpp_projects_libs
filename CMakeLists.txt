cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(rmglib VERSION 1.0.0)

if(WIN32)
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".dll")
elseif(UNIX)
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".lib")
endif()

# CACHE = it stores the variable in cache
# FORCE is passed as an argument to overwrite things stored in cache, 
# as on Vscode we would have to rebuild this from scrath
set(PERSONAL_BUILD_COMPUTER_GRAPHICS ON CACHE BOOL "Build computer graphics apps" FORCE)

add_subdirectory(vendors/googletest)
target_compile_definitions(gtest PUBLIC GTEST_LANG_CXX20)

include(CTest)
enable_testing()

add_subdirectory(util)

if(PERSONAL_BUILD_COMPUTER_GRAPHICS)
    add_subdirectory(computer_graphics)
endif()