project(ImageLib)
set(SOURCES "decode_jpg.cpp")
add_executable(decode_jpg "decode_jpg.cpp")

set_target_properties(decode_jpg PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(decode_jpg PRIVATE "../util/include")

# find_package(Boost REQUIRED)
# if(Boost_FOUND)   
#     target_link_libraries(decode_jpg Boost)
# endif()


add_subdirectory(fourier_transform)