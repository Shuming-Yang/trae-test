cmake_minimum_required(VERSION 3.10)
project(trae_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

add_executable(trae_test main.c main.h start.s)

add_executable(unit_test unit_test.c unit_test.h main.c main.h)
target_compile_definitions(unit_test PRIVATE TEST_BUILD)

add_executable(integrated_test integrated_test.c integrated_test.h main.c main.h)
target_compile_definitions(integrated_test PRIVATE TEST_BUILD)