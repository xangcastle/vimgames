#!/bin/bash
set -e

# Run Galaga screenshot test
echo "Running Galaga screenshot test..."
bazel run //galaga:galaga -- --test-screenshot
if grep -q "GALAGA" galaga_test.txt; then
    echo "Galaga screenshot test PASSED"
else
    echo "Galaga screenshot test FAILED"
    exit 1
fi

# Build all games first
# echo "Building games..."
# bazel build //hunter:hunter //pacman:pacman

# Run Hunter screenshot test
echo "Running Hunter screenshot test..."
./bazel-bin/hunter/hunter --test-screenshot
if grep -q "HUNTER" hunter_test.txt; then
    echo "Hunter screenshot test PASSED"
else
    echo "Hunter screenshot test FAILED"
    exit 1
fi

# Run Pacman screenshot test
echo "Running Pacman screenshot test..."
./bazel-bin/pacman/pacman --test-screenshot
# Check for map text since UI might be cut off in small terminal
if grep -q "genius" pacman_test.txt; then
    echo "Pacman screenshot test PASSED"
else
    echo "Pacman screenshot test FAILED"
    exit 1
fi

echo "All screenshot tests PASSED"
