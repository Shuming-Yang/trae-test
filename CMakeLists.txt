cmake_minimum_required(VERSION 3.10)
project(trae_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

add_executable(trae_test src/main.c src/start.s)
target_include_directories(trae_test PRIVATE inc)

add_executable(unit_test unit_test/unit_test.c src/main.c)
target_include_directories(unit_test PRIVATE inc unit_test)
target_compile_definitions(unit_test PRIVATE TEST_BUILD)

add_executable(integrated_test integration_test/integrated_test.c src/main.c)
target_include_directories(integrated_test PRIVATE inc integration_test)
target_compile_definitions(integrated_test PRIVATE TEST_BUILD)

if(ENABLE_COVERAGE)
    target_compile_options(unit_test PRIVATE --coverage -O0)
    target_link_options(unit_test PRIVATE --coverage)
    target_compile_options(integrated_test PRIVATE --coverage -O0)
    target_link_options(integrated_test PRIVATE --coverage)
endif()