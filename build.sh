#!/bin/bash
set -e

BUILD_DIR="build"

# Create the main build directory if it doesn't exist.
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

# Configure the main project (if needed).
cd "$BUILD_DIR"
echo "Configuring the project with CMake..."
cmake -G "Ninja" -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=RELEASE ..
ninja
cd ..
