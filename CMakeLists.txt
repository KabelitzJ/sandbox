cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(libsbx VERSION 0.1.0 LANGUAGES CXX)

string(TOUPPER ${CMAKE_PROJECT_NAME} _CMAKE_PROJECT_NAME_UPPER)
set(CMAKE_PROJECT_NAME_UPPER ${_CMAKE_PROJECT_NAME_UPPER} CACHE STRING "CMAKE_PROJECT_NAME_UPPER" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/dependencies")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/dependencies")

option(SBX_BUILD_DEMO "Build demo" ON)
message(STATUS "SBX_BUILD_DEMO: ${SBX_BUILD_DEMO}")

option(SBX_BUILD_SHARED "Build shared library" OFF)
message(STATUS "SBX_BUILD_SHARED: ${SBX_BUILD_SHARED}")

option(SBX_BUILD_TESTS "Build tests" On)
message(STATUS "SBX_BUILD_TESTS: ${SBX_BUILD_TESTS}")

option(SBX_CONSTEXPR_ENABLED "Enable constexpr" On)
message(STATUS "SBX_CONSTEXPR_ENABLED: ${SBX_CONSTEXPR_ENABLED}")

add_subdirectory(libsbx-units)
add_subdirectory(libsbx-utility)
add_subdirectory(libsbx-io)
add_subdirectory(libsbx-math)
add_subdirectory(libsbx-memory)
add_subdirectory(libsbx-signals)
add_subdirectory(libsbx-bitmaps)
add_subdirectory(libsbx-ecs)
add_subdirectory(libsbx-core)
add_subdirectory(libsbx-async)
add_subdirectory(libsbx-assets)
add_subdirectory(libsbx-devices)
add_subdirectory(libsbx-graphics)
add_subdirectory(libsbx-post)
add_subdirectory(libsbx-models)
add_subdirectory(libsbx-scenes)
# add_subdirectory(libsbx-audio)
add_subdirectory(libsbx-ui)
add_subdirectory(libsbx-shadows)
add_subdirectory(libsbx-physics)

if (${SBX_BUILD_DEMO})
  add_subdirectory(demo)
endif()
