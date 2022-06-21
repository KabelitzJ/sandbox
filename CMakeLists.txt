cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/external" "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/external")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
  sbx
  VERSION 0.0.1
  LANGUAGES CXX
)


add_subdirectory(external)

# Build library
add_subdirectory(sandbox)

# Build demo project
option(SBX_BUILD_DEMO "Enable building the demo project." ON)

if(SBX_BUILD_DEMO)
  add_subdirectory(demo)
endif()

# Build unit tests
option(SBX_BUILD_TESTS "Enable building unit tests." OFF)

if(SBX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Generate documentations
option(SBX_GENERATE_DOCS "Enable generating documentations." OFF)

if(SBX_GENERATE_DOCS)
  add_subdirectory(docs)
endif()
