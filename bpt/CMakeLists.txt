project(bpt)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/data)
add_executable(bpt ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
