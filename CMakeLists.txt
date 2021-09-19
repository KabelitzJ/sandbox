cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sbx CXX)

add_subdirectory(sandbox)
add_subdirectory(demo)
