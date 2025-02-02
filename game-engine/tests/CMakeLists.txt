cmake_minimum_required(VERSION 3.31.4)

include(FetchContent)

# Download GoogleTest.
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Manually add the include directory.
# Adjust the path as necessary; here we assume tests/ is at the same level as include/
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Create the test executable.
add_executable(xiangqi_tests
    test_game.cc
)

# Link the engine library and GoogleTest.
# If xiangqi_engine is built as part of another project, you might have to link it in a different way,
# such as using find_package or add it as an imported target.
target_link_libraries(xiangqi_tests
    PRIVATE xiangqi_engine
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(xiangqi_tests)
