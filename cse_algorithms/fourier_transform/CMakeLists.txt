add_executable(fourier_transform main.cpp)

target_include_directories(fourier_transform PUBLIC  "include" "../../util/include" "../vendors/phmap_parallel_hashmap")
set_target_properties(fourier_transform PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(fourier_transform PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)

find_package(Threads REQUIRED)
target_link_libraries(fourier_transform PRIVATE Threads::Threads)
