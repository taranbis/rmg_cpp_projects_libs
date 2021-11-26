set (CMAKE_MODULE_PATH 
  "${CMAKE_SOURCE_DIR}/cmake" 
  ${CMAKE_MODULE_PATH}
)

find_package (CUDA 10.0 REQUIRED)
project(ray_tracing_with_cuda LANGUAGES C CXX CUDA)

# add_library(ray_tracing_library STATIC ray.hpp vec3.hpp)
# set_target_properties(ray_tracing_library PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# set_target_properties(ray_tracing_library PROPERTIES LINKER_LANGUAGE CXX)

add_executable(ray_tracing ray_tracing.cu)
set_target_properties(ray_tracing PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(ray_tracing PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
include_directories(PUBLIC "../../util/include")
# target_link_libraries(ray_tracing PRIVATE ray_tracing_library)

if(UNIX)
  add_definitions(-DUNIX)
endif(UNIX)


# set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# add_subdirectory(cutil)
# add_subdirectory(shrutil)
# add_subdirectory(src)

# CUDA_BUILD_CLEAN_TARGET()