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
set(PERSONAL_BUILD_IMAGE_LIBRARY OFF CACHE BOOL "Building Generic Image Library" FORCE)
set(PERSONAL_BUILD_IOT_PROJECT OFF CACHE BOOL "Build IoT Project" FORCE)
set(PERSONAL_BUILD_SIMULATIONS ON CACHE BOOL "Build CSE Simulation Algorithms" FORCE)

if(WIN32)
    set(BOOST_ROOT "C:/local/boost_1_73_0")
endif()

add_subdirectory(vendors/googletest)
target_compile_definitions(gtest PUBLIC GTEST_LANG_CXX20)
add_subdirectory(util)

include(CTest)
enable_testing()


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


# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
