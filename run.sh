#!/bin/bash

# Usage: ./run.sh [test|bench|both]

INPUT="$1"
BUILD_DIR="$(dirname "$0")/build"

if [[ -z "$INPUT" ]]; then
    echo "Usage: $0 [test|bench|both]"
    exit 1
fi

cd "$BUILD_DIR" || { echo "Build directory not found."; exit 1; }

if [[ "$INPUT" == "test" || "$INPUT" == "both" ]]; then
    ctest
fi

if [[ "$INPUT" == "bench" || "$INPUT" == "both" ]]; then
    ./src/bench/queue_benchmark
fi