

set(SOURCES bubble_sort.cpp)

add_executable(bubble_sort ${SOURCES})

target_include_directories(bubble_sort PUBLIC "../../util/include")
set_target_properties(bubble_sort PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# find_package(Sfml)


find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

#this didn't work 
# target_link_libraries(bubble_sort ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_link_libraries(bubble_sort PRIVATE sfml-window sfml-audio sfml-graphics sfml-network sfml-system)


