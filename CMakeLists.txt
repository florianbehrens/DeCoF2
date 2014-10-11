cmake_minimum_required(VERSION 2.8)
project(decof2)

find_package(Boost 1.54 REQUIRED COMPONENTS system)

add_definitions(-std=c++11)
include_directories(include include/decof src/core)
add_subdirectory(src)
add_subdirectory(example)
