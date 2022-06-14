# Create folder for build output
mkdir -p build

# Generate the cmake project
cmake . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DSBX_BUILD_TESTS=False

# Build the project
cmake --build build
