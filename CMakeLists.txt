cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(sbx CXX)

add_subdirectory(sandbox)
add_subdirectory(demo)
