cmake_minimum_required(VERSION 3.21)

project(do_magic)

add_executable(do_magic do_magic.cpp)
target_include_directories(do_magic PUBLIC ./)
