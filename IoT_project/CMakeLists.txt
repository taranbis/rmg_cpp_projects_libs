cmake_minimum_required(VERSION 3.12)
project(Kinexon VERSION 0.1.0)

# add_executable(Test "Test.cpp")
# add_executable(KalmanFilterTest "KalmanFilter.cpp")

add_executable(DataGenerator "DataGenerator.cpp")
add_executable(DataProcessor "DataProcessor.cpp")

# set(CMAKE_BUILD_TYPE "Debug")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/eigen/cmake")

# include_directories(./eigen)
find_package(PahoMqttCpp REQUIRED)
find_package(Threads REQUIRED)
find_package (Eigen3 3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})


# target_link_libraries(Test Threads::Threads)
target_link_libraries(DataGenerator Threads::Threads)
target_link_libraries(DataProcessor Threads::Threads)

set_target_properties (DataGenerator PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
set_target_properties (DataProcessor PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_include_directories (DataGenerator PRIVATE "../../include")
target_include_directories (DataProcessor PRIVATE "../../include")

target_link_libraries(DataProcessor PahoMqttCpp::paho-mqttpp3)
target_link_libraries(DataGenerator PahoMqttCpp::paho-mqttpp3)

target_link_libraries(DataProcessor Eigen3::Eigen)
target_link_libraries(DataGenerator Eigen3::Eigen)