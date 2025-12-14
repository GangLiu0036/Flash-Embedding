cmake_minimum_required(VERSION 3.20)
project(FlashEmbed-Cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Useful for clang-tidy/LSP

# Find Pybind11
find_package(pybind11 REQUIRED CONFIG)

# Source and Include directories
include_directories(include)

# Define the C++ library
# Added new kernel sources
add_library(flash_embed_lib STATIC
    src/flash_embed.cpp
    src/kernels/scalar_ops.cpp
    src/kernels/avx2_ops.cpp
)

# 针对 AVX2 源码文件启用 -mavx2 -mfma 编译标志
# 确保编译器能识别 intrinsics，同时不会将整个项目强制设为 AVX2 (保持粒度)
set_source_files_properties(src/kernels/avx2_ops.cpp PROPERTIES COMPILE_FLAGS "-mavx2 -mfma")

# Define the Python extension module
pybind11_add_module(_core src/bindings.cpp)
target_link_libraries(_core PRIVATE flash_embed_lib)

# Install directly to the 'flash_embed' package directory
install(TARGETS _core DESTINATION flash_embed)

# ==========================================
# Testing (GTest)
# ==========================================
enable_testing()

# Check if GTest is installed (e.g. via Conda), otherwise fetch it
find_package(GTest)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found locally, fetching from git...")
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Create the test executable
add_executable(cpp_tests tests/cpp/test_main.cpp)
target_link_libraries(cpp_tests PRIVATE flash_embed_lib GTest::gtest_main)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(cpp_tests)

# ==========================================
# Benchmarking (Google Benchmark)
# ==========================================
find_package(benchmark)
if(NOT benchmark_FOUND)
    message(STATUS "Google Benchmark not found locally, fetching from git...")
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
endif()

add_executable(cpp_benchmarks benchmarks/cpp/benchmark_ops.cpp)
target_link_libraries(cpp_benchmarks PRIVATE flash_embed_lib benchmark::benchmark)

# 关键：禁用 Benchmark 目标的自动向量化，以确保 Scalar Baseline 是公平的
# -fno-tree-vectorize: 禁止 GCC/Clang 自动将标量循环优化为 SIMD
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cpp_benchmarks PRIVATE -fno-tree-vectorize -mavx2 -mfma)
endif()
