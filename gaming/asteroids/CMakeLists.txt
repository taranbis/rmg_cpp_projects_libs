add_executable(asteroids_game main.cpp game.cpp)

target_include_directories(asteroids_game PUBLIC "../../util/include")
set_target_properties(asteroids_game PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(asteroids_game PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)

find_package(Threads REQUIRED)
target_link_libraries(asteroids_game PRIVATE Threads::Threads)


add_executable(sfml_test test.cpp)

target_include_directories(sfml_test PUBLIC "../../util/include")
set_target_properties(sfml_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(sfml_test PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)
