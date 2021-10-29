set(SOURCES csv.cpp test_csv_parser.cpp)
set(LIB_SOURCES csv.cpp)
set(HEADERS csv.hpp)

project(CSVParser)
#add_executable(csv ${SOURCES})
#set_target_properties(csv PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

add_library(csvLib ${LIB_SOURCES} ${HEADERS} )
set_target_properties(csvLib PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

target_include_directories(csvLib PUBLIC "../include")