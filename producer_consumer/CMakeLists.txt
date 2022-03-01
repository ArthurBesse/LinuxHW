cmake_minimum_required(VERSION 3.16)
project(producer_consumer)

set(CMAKE_CXX_STANDARD 20)

add_executable(producer_consumer producer_consumer.cpp)

target_include_directories(producer_consumer PRIVATE ../)

target_link_libraries(producer_consumer pthread)
