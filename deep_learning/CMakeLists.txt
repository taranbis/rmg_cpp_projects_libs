##### Get all libraries: for this project Torch, MXnet and Eigen
find_package(Torch REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/libraries/libtorch/share/cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../eigen/cmake")
find_package (Eigen3 3.3 REQUIRED)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../Modules/)
list(APPEND MXNET_INCLUDE_SEARCH_PATHS /usr/local/include/mxnet-cpp)
list(APPEND MXNET_LIB_SEARCH_PATHS /usr/local/lib)
find_package(MxNet REQUIRED)

add_executable(market_prediction market_prediction.cpp)
target_include_directories(market_prediction PUBLIC "./include" "../util/csv" "../util/include")
target_link_libraries(market_prediction csvLib)
set_target_properties(market_prediction PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

set(DL_SOURCES  dl.cpp 
                src/linear.cpp 
  #              src/layer_utils.cpp 
               src/data_utils.cpp
    )
add_executable(deep_learning_lib ${DL_SOURCES})
target_include_directories(deep_learning_lib PUBLIC "./include" "../util/csv" ${EIGEN3_INCLUDE_DIR} "../util/include")
target_link_libraries(deep_learning_lib Eigen3::Eigen)
set_target_properties(deep_learning_lib PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

###### PyTorch Test #####
add_executable(torch_test torch_test.cpp)
target_link_libraries(torch_test "${TORCH_LIBRARIES}")
set_target_properties(torch_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(torch_test PUBLIC ${CMAKE_CURRENT_LIST_DIR}/libraries/libtorch/include/torch/csrc/api/include "../util/include")

###### MXNET Test #####
add_executable(mxnet_test mxnet_test.cpp)
target_include_directories(mxnet_test PUBLIC ${MxNet_INCLUDE_DIRS} "../util/include")
target_link_libraries(mxnet_test ${MxNet_LIB})
set_target_properties(mxnet_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

add_subdirectory(facial_keypoints)