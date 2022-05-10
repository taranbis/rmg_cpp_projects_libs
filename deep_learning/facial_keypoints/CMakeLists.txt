##### Get all libraries: for this project Torch, MXnet and Eigen
find_package(Torch REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/libraries/libtorch/share/cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../eigen/cmake")
find_package (Eigen3 3.3 REQUIRED)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../Modules/)
list(APPEND MXNET_INCLUDE_SEARCH_PATHS /usr/local/include/mxnet-cpp)
list(APPEND MXNET_LIB_SEARCH_PATHS /usr/local/lib)
find_package(MxNet REQUIRED)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

# CMakeLists.txt
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Modules/")


#need version version 3.10.0
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

set(SOURCES facial_keypoints.cpp)
add_executable(facial_keypoints ${SOURCES})
set_target_properties(facial_keypoints PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(facial_keypoints PRIVATE Eigen3::Eigen)
target_link_libraries(facial_keypoints PRIVATE "${TORCH_LIBRARIES}")
target_link_libraries(facial_keypoints PRIVATE Threads::Threads)
target_link_libraries(facial_keypoints PRIVATE ${OpenCV_LIBS})
target_link_libraries(facial_keypoints PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(facial_keypoints PUBLIC "../include" "../../util/csv" ${EIGEN3_INCLUDE_DIR} "../../util/include")
target_include_directories(facial_keypoints PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../libraries/libtorch/include/torch/csrc/api/include)
