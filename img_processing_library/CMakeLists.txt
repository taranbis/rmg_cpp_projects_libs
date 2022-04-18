project(ImageLib)

add_executable(decode_jpg "decode_jpg.cpp")
set_target_properties(decode_jpg PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(decode_jpg PRIVATE "../util/include")

add_executable(png_decoder "png_decoder.cpp")
set_target_properties(png_decoder PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(png_decoder PRIVATE "../util/include")

# find_package(Boost REQUIRED)
# if(Boost_FOUND)   
#     target_link_libraries(decode_jpg Boost)
# endif()


add_subdirectory(fourier_transform)