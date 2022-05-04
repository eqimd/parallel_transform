cmake_minimum_required(VERSION 3.10)

project(parallel_transform)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("src")
include_directories("src/futex")
include_directories("src/transform")

add_executable(
        ${PROJECT_NAME}
        main.cpp
        src/futex/futex.cpp
        src/transform/transform.h
)
