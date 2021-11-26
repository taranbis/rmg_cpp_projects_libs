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
set(PERSONAL_BUILD_COMPUTER_GRAPHICS ON CACHE BOOL "Build computer graphics apps" FORCE)
set(PERSONAL_BUILD_IMAGE_LIBRARY ON CACHE BOOL "Building Generic Image Library" FORCE)
set(PERSONAL_BUILD_IOT_PROJECT OFF CACHE BOOL "Build IoT Project" FORCE)
set(PERSONAL_BUILD_SIMULATIONS ON CACHE BOOL "Build CSE Simulation Algorithms" FORCE)
set(PERSONAL_BUILD_GAMING ON CACHE BOOL "Building Gaming" FORCE)
set(PERSONAL_BUILD_DEEP_LEARNING ON CACHE BOOL "Build Deep Learning Library" FORCE)

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

if(PERSONAL_BUILD_COMPUTER_GRAPHICS)
    add_subdirectory(computer_graphics)
endif()

if(PERSONAL_BUILD_IMAGE_LIBRARY)
    add_subdirectory(img_processing_library)
endif()

if(PERSONAL_BUILD_SIMULATIONS)
    add_subdirectory(cse_algorithms)
endif()

if(UNIX AND PERSONAL_BUILD_IOT_PROJECT)
    add_subdirectory(IoT_project)
endif()

if(PERSONAL_BUILD_GAMING)
    add_subdirectory(gaming)
endif()

if(PERSONAL_BUILD_DEEP_LEARNING)
    add_subdirectory(deep_learning)
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
