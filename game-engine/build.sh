#!/bin/bash
# build.sh
# This script configures and builds the XiangqiGameEngine library using CMake.

# Exit immediately if a command exits with a non-zero status.
set -e

# Define the build directory name.
BUILD_DIR="build"

# Create the build directory if it doesn't exist.
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

# Change to the build directory.
cd "$BUILD_DIR"

# Run CMake to configure the project.
# You can change Release to Debug if needed.
echo "Configuring the project with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project.
echo "Building the project..."
cmake --build .

# Optional: Return to the project root directory.
cd ..

echo "Build complete."
