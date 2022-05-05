cmake_minimum_required(VERSION 3.10)

project(parallel_transform)

set(CMAKE_CXX_STANDARD 20)

include_directories("src")
include_directories("src/transform")
include_directories("src/range_scheduler")
include_directories("src/vector_shared")

add_library(transform SHARED
        src/transform/transform.h
        src/transform/vector_shared.h
        src/range_scheduler/range_scheduler.cpp
        src/range_scheduler/range_scheduler.h
)

link_libraries(transform)

add_executable(test_eq_1 tests/test_eq_1.cpp)
add_executable(test_eq_2 tests/test_eq_2.cpp)
add_executable(test_eq_3 tests/test_eq_3.cpp)

enable_testing()

add_test(
  NAME test_eq_1
  COMMAND $<TARGET_FILE:test_eq_1>
)
add_test(
  NAME test_eq_2
  COMMAND $<TARGET_FILE:test_eq_2>
)
add_test(
  NAME test_eq_3
  COMMAND $<TARGET_FILE:test_eq_3>
)
