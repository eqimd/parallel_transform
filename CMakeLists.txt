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

add_executable(eq_dyn_1 tests/eq_dyn_1.cpp)
add_executable(eq_dyn_2 tests/eq_dyn_2.cpp)
add_executable(eq_dyn_3 tests/eq_dyn_3.cpp)
add_executable(eq_static tests/eq_static.cpp)
add_executable(proc_parallel_1 tests/proc_parallel_1.cpp)
add_executable(proc_parallel_2 tests/proc_parallel_2.cpp)

enable_testing()

add_test(
  NAME eq_dyn_1
  COMMAND $<TARGET_FILE:eq_dyn_1>
)
add_test(
  NAME eq_dyn_2
  COMMAND $<TARGET_FILE:eq_dyn_2>
)
add_test(
  NAME eq_dyn_3
  COMMAND $<TARGET_FILE:eq_dyn_3>
)
add_test(
  NAME eq_static
  COMMAND $<TARGET_FILE:eq_static>
)
add_test(
  NAME proc_parallel_1
  COMMAND $<TARGET_FILE:proc_parallel_1>
)
add_test(
  NAME proc_parallel_2
  COMMAND $<TARGET_FILE:proc_parallel_2>
)
