set(SOURCES market_prediction.cpp)

project(market_prediction)
add_executable(market_prediction ${SOURCES})

set_target_properties(market_prediction PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(market_prediction PUBLIC "./include" "../util/csv")
target_link_libraries(market_prediction csvLib)

include_directories(PUBLIC "../util/include")


set(DL_SOURCES  dl.cpp 
                src/linear.cpp 
  #              src/layer_utils.cpp 
               src/data_utils.cpp
    )
project(DeepLearning)

add_executable(DeepLearning ${DL_SOURCES})
target_include_directories(DeepLearning PUBLIC "./include" "../util/csv")
set_target_properties(DeepLearning PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../eigen/cmake")
find_package (Eigen3 3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
target_link_libraries(DeepLearning Eigen3::Eigen)


# add_executable(TorchTest torch_test.cpp)
# find_package(Torch REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/libraries/libtorch/share/cmake)
# # find_package(Torch REQUIRED)
# target_link_libraries(TorchTest "${TORCH_LIBRARIES}")
# set_target_properties(TorchTest PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../Modules/)
list(APPEND MXNET_INCLUDE_SEARCH_PATHS /usr/local/include/mxnet-cpp)
list(APPEND MXNET_LIB_SEARCH_PATHS /usr/local/lib)
add_executable(mxnetTest mxnet_test.cpp)
find_package(MxNet REQUIRED)
# find_package(Torch REQUIRED)
include_directories(${MxNet_INCLUDE_DIRS})
target_link_libraries(mxnetTest ${MxNet_LIB})
set_target_properties(mxnetTest PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

