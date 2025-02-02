cmake_minimum_required(VERSION 3.10)
project(XiangqiGameEngine LANGUAGES CXX)

# Set the C++ standard (adjust as needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally, add options for building tests, examples, etc.

# Add the subdirectory that contains the library’s sources.
add_subdirectory(src)
