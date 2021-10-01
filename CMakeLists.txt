cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project(sbx CXX)

# Build main library
add_subdirectory(sandbox)

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
