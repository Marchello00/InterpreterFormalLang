cmake_minimum_required(VERSION 3.13)
project(interpreter)

add_subdirectory(src)
target_include_directories(interpreter PUBLIC lib)