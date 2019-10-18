cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(bayes_toolbox CXX)

# Compilation options
option(COVERAGE_MODE "For coverage mode using g++ " OFF) #OFF by default
option(DEBUG_MODE "Debug mode (with asserts and such) " OFF) #OFF by default
set(ALWAYSON_CXX_FLAGS "--std=c++14 -Wall -Wextra -Wpedantic -Wfatal-errors $ENV{EXTRA_CXX_FLAGS}")
if(COVERAGE_MODE)
    set(CMAKE_CXX_FLAGS "-O0 -fprofile-arcs -ftest-coverage ${ALWAYSON_CXX_FLAGS}") # coverage mode
    message("-- INFO: Compiling in coverage mode.\n-- INFO: flags are: " ${CMAKE_CXX_FLAGS})
elseif(DEBUG_MODE)
    set(CMAKE_CXX_FLAGS "-O0 -g ${ALWAYSON_CXX_FLAGS}") # debug mode
    message("-- INFO: Compiling in debug mode.\n-- INFO: flags are: " ${CMAKE_CXX_FLAGS})
else()
    set(CMAKE_CXX_FLAGS "-O3 -DNDEBUG ${ALWAYSON_CXX_FLAGS}") # release mode
    message("-- INFO: Compiling in release mode.\n-- INFO: flags are: " ${CMAKE_CXX_FLAGS})
endif(COVERAGE_MODE)

include_directories("src")
include_directories("utils")
include_directories("utils/tagged_tuple/utils")

add_executable(basic_test "src/test.cpp")
add_executable(new_test "src/new_test.cpp")
add_executable(poisson_gamma "src/model_example.cpp")
add_executable(example_1_1 "examples/example_1_1.cpp")


add_executable(model_example "src/model_example.cpp")
add_executable(example_1_1b "examples/example_1_1b.cpp")
add_executable(unif_bern "examples/unif_bern.cpp")
# add_executable(poisson_gamma_mixture "src/model_example_2.cpp")
