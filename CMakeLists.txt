cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(rmglib VERSION 1.0.0)

# set(CMAKE_BUILD_TYPE Debug)

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
set(RMG_BUILD_COMPUTER_GRAPHICS ON CACHE BOOL "Build computer graphics apps" FORCE)
set(RMG_BUILD_IMAGE_LIBRARY ON CACHE BOOL "Building Generic Image Library" FORCE)
set(RMG_BUILD_IOT_PROJECT ON CACHE BOOL "Build IoT Project" FORCE)
set(RMG_BUILD_SIMULATIONS ON CACHE BOOL "Build CSE Simulation Algorithms" FORCE)
set(RMG_BUILD_GAMING ON CACHE BOOL "Building Gaming" FORCE)
set(RMG_BUILD_DEEP_LEARNING ON CACHE BOOL "Build Deep Learning Library" FORCE)

#set(OPENCV_ENABLE_NONFREE ON)

if(WIN32)
    set(BOOST_ROOT "C:/local/boost_1_73_0")
endif()

add_subdirectory(vendors/googletest)
target_compile_definitions(gtest PUBLIC GTEST_LANG_CXX20)
add_subdirectory(util)

include(CTest)
enable_testing()

add_subdirectory(test)
add_subdirectory(allocator)

if(RMG_BUILD_COMPUTER_GRAPHICS)
    add_subdirectory(computer_graphics)
endif()

if(RMG_BUILD_IMAGE_LIBRARY)
    add_subdirectory(img_processing_library)
endif()

if(RMG_BUILD_SIMULATIONS)
    add_subdirectory(cse_algorithms)
endif()

if(UNIX AND RMG_BUILD_IOT_PROJECT)
    add_subdirectory(IoT_project)
endif()

if(RMG_BUILD_GAMING)
    add_subdirectory(gaming)
endif()

if(RMG_BUILD_DEEP_LEARNING)
    add_subdirectory(deep_learning)
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
