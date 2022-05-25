cmake_minimum_required(VERSION 3.16)
project(thread_pool)

set(CMAKE_CXX_STANDARD 20)

add_executable(thread_pool thread_pool.cpp)

target_include_directories(thread_pool PRIVATE ../)

target_link_libraries(thread_pool pthread)
