cmake_minimum_required(VERSION 3.31.4)
project(XiangqiGameEngine LANGUAGES CXX)

# Set the C++ standard (adjust as needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the subdirectory that contains the library’s sources.
add_subdirectory(src)

# Enable testing support.
enable_testing()

# --- Configure GoogleTest using FetchContent ---
include(FetchContent)

# Download GoogleTest.
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

# Optionally, for Windows, enforce shared CRT.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make the content available.
FetchContent_MakeAvailable(googletest)

# --- Create the test executable ---

# Specify the test source file(s). Adjust the path if needed.
add_executable(xiangqi_tests
    tests/test_game.cc
    tests/test_possible_moves.cc
)

# Link the engine library and GoogleTest libraries.
target_link_libraries(xiangqi_tests
    PRIVATE xiangqi_engine
    gtest_main
)

# Enable CTest to run the tests.
include(GoogleTest)
gtest_discover_tests(xiangqi_tests)
