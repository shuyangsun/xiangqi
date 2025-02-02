#!/bin/bash
set -e

BUILD_DIR="build"
TEST_EXE="xiangqi_tests"

if [ ! -x "./${BUILD_DIR}/${TEST_EXE}" ]; then
    echo "Error: Test executable '${TEST_EXE}' not found. Please ensure it is built."
    exit 1
fi

echo "Running tests using the GoogleTest executable..."
./${BUILD_DIR}/${TEST_EXE} --gtest_color=yes

echo "Tests complete."
