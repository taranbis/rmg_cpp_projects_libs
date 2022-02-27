find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
find_package(Threads REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

add_executable(fourier_transform main.cpp)
target_include_directories(fourier_transform PUBLIC  "include" "../../util/include" "../vendors/phmap_parallel_hashmap")
set_target_properties(fourier_transform PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(fourier_transform PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system Threads::Threads)

add_executable(fourier_drawing_circle fourier_drawing_circle.cpp)
target_include_directories(fourier_drawing_circle PUBLIC  "include" "../../util/include" "../vendors/phmap_parallel_hashmap")
set_target_properties(fourier_drawing_circle PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(fourier_drawing_circle PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system Threads::Threads)

add_executable(fourier_drawing_general_path fourier_drawing_general_path.cpp)
target_include_directories(fourier_drawing_general_path PUBLIC  "include" "../../util/include" "../vendors/phmap_parallel_hashmap")
set_target_properties(fourier_drawing_general_path PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(fourier_drawing_general_path PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system Threads::Threads)