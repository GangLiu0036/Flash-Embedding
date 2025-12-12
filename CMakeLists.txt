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
add_library(flash_embed_lib STATIC
    src/flash_embed.cpp
)

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
