cmake_minimum_required(VERSION 3.20)

project(parallel-cpu-lab VERSION 1.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Libraries
find_package(benchmark CONFIG REQUIRED)
find_package(Threads REQUIRED)

if (BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
endif()

# General compiler flags
add_library(compiler_options INTERFACE)

target_compile_options(compiler_options INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -march=native
)

# Reduction implementation
add_library(parallel_reduction src/reduction.cpp)

target_include_directories(parallel_reduction PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(parallel_reduction PUBLIC
    compiler_options
    Threads::Threads
)

# Reduction benchmark
add_executable(reduction_bench benchmarks/reduction_bench.cpp)

target_link_libraries(reduction_bench PRIVATE 
    parallel_reduction
    benchmark::benchmark
    benchmark::benchmark_main
)

# Reduction tests
if (BUILD_TESTING)
    add_executable(reduction_tests
        tests/reduction_test.cpp
    )

    target_link_libraries(reduction_tests PRIVATE
        parallel_reduction
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(reduction_tests)
endif()
