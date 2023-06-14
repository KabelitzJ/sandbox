cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(libsbx)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/dependencies)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

option(SPIRV_CROSS_CLI "Build the CLI binary. Requires SPIRV_CROSS_STATIC." OFF)
option(SPIRV_CROSS_ENABLE_TESTS "Enable SPIRV-Cross tests." OFF)

add_subdirectory(libsbx-units)
add_subdirectory(libsbx-utility)
add_subdirectory(libsbx-async)
add_subdirectory(libsbx-io)
add_subdirectory(libsbx-math)
add_subdirectory(libsbx-memory)
add_subdirectory(libsbx-core)
add_subdirectory(libsbx-devices)
add_subdirectory(libsbx-graphics)
add_subdirectory(libsbx-ecs)
add_subdirectory(libsbx-scenes)
add_subdirectory(libsbx-scripting)
add_subdirectory(libsbx-signal)
add_subdirectory(libsbx-models)

add_subdirectory(demo)
