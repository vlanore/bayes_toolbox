cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(bayes_toolbox CXX)

# Compilation options
set(CMAKE_CXX_FLAGS "--std=c++14 -Wall -Wextra -Wfatal-errors -g")

include_directories("src")
include_directories("utils")

add_executable(all_tests "src/test.cpp")
