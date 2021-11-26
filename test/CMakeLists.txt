

add_executable(test1 test1.cpp)
target_include_directories(test1 PUBLIC ../util/include)
set_target_properties(test1 PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)