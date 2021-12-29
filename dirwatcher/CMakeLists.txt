cmake_minimum_required(VERSION 3.16)

project(directory_watcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(directory_watcher directory_watcher.cpp)

target_include_directories(directory_watcher PRIVATE ../)

target_link_libraries(directory_watcher pthread)
