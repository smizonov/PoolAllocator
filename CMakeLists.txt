cmake_minimum_required(VERSION 3.15)

project(Allocator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
