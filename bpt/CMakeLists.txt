project(bpt)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
#add_executable(code ${CMAKE_CURRENT_SOURCE_DIR}/src/main_list.cpp)
add_executable(code ${CMAKE_CURRENT_SOURCE_DIR}/src/main_bpt.cpp)
add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp)