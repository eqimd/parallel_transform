cmake_minimum_required(VERSION 3.10)

project(parallel_transform)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("src")
include_directories("src/transform")
include_directories("src/range_scheduler")
include_directories("src/vector_shared")

set(
        transform_src
        src/transform/transform.h
        src/transform/vector_shared.h
        src/range_scheduler/range_scheduler.h
)

add_executable(
        ${PROJECT_NAME}
        ${transform_src}
        src/range_scheduler/range_scheduler.cpp
        src/range_scheduler/range_scheduler.h
        main.cpp
)
