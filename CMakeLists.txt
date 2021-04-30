cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(sbx CXX)

add_subdirectory(sandbox)
add_subdirectory(demo)
