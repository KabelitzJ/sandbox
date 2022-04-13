#!/bin/bash

# Create folder for build output
mkdir -p build

# Use conan to install the dependencies
conan install . -if build/conan --build=missing

# Generate the cmake project
cmake . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DSBX_BUILD_TESTS=False

# Build the project
cmake --build build
