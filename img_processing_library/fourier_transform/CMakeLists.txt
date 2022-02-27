

set(SOURCES "main.cpp")
add_executable(img_proc_fourier_transform ${SOURCES})

set_target_properties(img_proc_fourier_transform PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(img_proc_fourier_transform PUBLIC  "include" "../../util/include" "../vendors/phmap_parallel_hashmap")
