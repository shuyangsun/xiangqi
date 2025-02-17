#!/bin/bash
set -e

BUILD_DIR="build"
BENCH_EXE="xiangqi_benchmarks"

if [ ! -x "./${BUILD_DIR}/${BENCH_EXE}" ]; then
    echo "Error: Benchmark executable '${BENCH_EXE}' not found. Please ensure it is built."
    exit 1
fi

echo "Running benchmarks using the Google Benchmark executable..."
./${BUILD_DIR}/${BENCH_EXE}

echo "Benchmark complete."
