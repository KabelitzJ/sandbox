cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/conan" "${CMAKE_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/conan")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  sbx
  VERSION 0.0.1
  LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build library
add_subdirectory(sandbox)

# Install library
# TODO: Add install target

# Build demo project
option(SBX_BUILD_DEMO "Enable building the demo project." ON)

if (SBX_BUILD_DEMO)
  add_subdirectory(demo)
endif()

# Build unit tests
option(SBX_BUILD_TESTS "Enable building unit tests." OFF)

if (SBX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Generate documentations
option(SBX_GENERATE_DOCS "Enable generating documentations." OFF)

if (SBX_GENERATE_DOCS)
  add_subdirectory(docs)
endif()
