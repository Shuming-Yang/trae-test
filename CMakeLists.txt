cmake_minimum_required(VERSION 3.10)
project(trae_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Disable compile_commands.json generation to avoid copy warning
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
 
add_executable(trae_test main.c main.h start.s)
