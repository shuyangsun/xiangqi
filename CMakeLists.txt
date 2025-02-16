cmake_minimum_required(VERSION 3.31.4)

project(XiangqiGameEngine LANGUAGES C CXX Swift)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(XIANGQI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(XIANGQI_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(XIANGQI_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(XIANGQI_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${XIANGQI_INCLUDE_DIR})

# ---------------------- Swift Library ----------------------

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Verify that we have a new enough compiler
    if("${CMAKE_Swift_COMPILER_VERSION}" VERSION_LESS 5.9)
        message(FATAL_ERROR "Bidirectional C++ Interop requires Swift 5.9 or greater. Have ${CMAKE_Swift_COMPILER_VERSION}")
    endif()

    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND
        NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        message(FATAL_ERROR "Project requires building with Clang. Have ${CMAKE_CXX_COMPILER_ID}")
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

    # Set up swiftrt.o and runtime library search paths
    include(InitializeSwift)
    # cmake/modules/AddSwift.cmake provides the function for creating the Swift to
    # C++ bridging header
    include(AddSwift)

    set(CMAKE_OSX_DEPLOYMENT_TARGET 15.0)
endif()

# ---------------------- Library and Binary ----------------------

add_subdirectory("${XIANGQI_LIB_DIR}/xiangqi")
# add_executable(xiangqi_app_ascii "${XIANGQI_SRC_DIR}/main.cc")
# target_link_libraries(xiangqi_app_ascii PRIVATE xiangqi_game_lib)

# ---------------------- GoogleTest ----------------------

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
    xiangqi_tests
    tests/test_board.cc
    tests/test_possible_moves.cc
    tests/test_game.cc
)
target_link_libraries(
    xiangqi_tests
    PRIVATE xiangqi_board_lib
    PRIVATE xiangqi_game_lib
    gtest_main
)
add_test(NAME xiangqi_test COMMAND xiangqi_test)
