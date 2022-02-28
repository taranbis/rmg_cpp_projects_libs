add_executable(asteroids_game main.cpp game.cpp)
set_target_properties(asteroids_game PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(SFML COMPONENTS system window graphics network audio REQUIRED)

target_include_directories(asteroids_game PRIVATE ${SFML_INCLUDE_DIR})
target_include_directories(asteroids_game PUBLIC "../../util/include")
target_link_libraries(asteroids_game PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)
target_link_libraries(asteroids_game PRIVATE Threads::Threads)


add_executable(sfml_test test.cpp)
target_include_directories(sfml_test PUBLIC "../../util/include")
set_target_properties(sfml_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(sfml_test PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(sfml_test PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)