cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(libsbx VERSION 0.1.0 LANGUAGES CXX)

# string(TOUPPER ${CMAKE_PROJECT_NAME} _CMAKE_PROJECT_NAME_UPPER)
# We use SBX instead of LIBSBX since its the convention for macros in the project.
set(CMAKE_PROJECT_NAME_UPPER "SBX" CACHE STRING "CMAKE_PROJECT_NAME_UPPER" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# if (NOT DEFINED ENV{VULKAN_SDK})
#   message(FATAL_ERROR "VULKAN_SDK environment variable not set. Please set it to the path of your Vulkan SDK installation.")
# endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/dependencies" "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_BINARY_DIR}/dependencies")

option(SBX_BUILD_DEMO "Build demo" ON)
message(STATUS "SBX_BUILD_DEMO: ${SBX_BUILD_DEMO}")

option(SBX_BUILD_SHARED "Build shared library" OFF)
message(STATUS "SBX_BUILD_SHARED: ${SBX_BUILD_SHARED}")

option(SBX_BUILD_TESTS "Build tests" On)
message(STATUS "SBX_BUILD_TESTS: ${SBX_BUILD_TESTS}")

option(SBX_CONSTEXPR_ENABLED "Enable constexpr" On)
message(STATUS "SBX_CONSTEXPR_ENABLED: ${SBX_CONSTEXPR_ENABLED}")

option(SBX_USE_PROFILER "Use profiler" On)
message(STATUS "SBX_USE_PROFILER: ${SBX_USE_PROFILER}")

string(TIMESTAMP COMPILE_TIMESTAMP %Y%m%d-%H%M%S)
set(SBX_COMPILE_TIMESTAMP "\"${COMPILE_TIMESTAMP}\"" CACHE STRING "Compile timestamp" FORCE)
add_compile_definitions(SBX_COMPILE_TIMESTAMP=${SBX_COMPILE_TIMESTAMP})

# Get current commit hash
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE SBX_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(SBX_COMMIT_HASH "\"${SBX_COMMIT_HASH}\"" CACHE STRING "Git commit hash" FORCE)
add_compile_definitions(SBX_COMMIT_HASH=${SBX_COMMIT_HASH})

# Get current branch name
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE SBX_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(SBX_BRANCH "\"${SBX_BRANCH}\"" CACHE STRING "Git branch" FORCE)
add_compile_definitions(SBX_BRANCH=${SBX_BRANCH})

# Optionally print them
message(STATUS "Git branch: ${GIT_BRANCH}")
message(STATUS "Git commit: ${GIT_COMMIT_HASH}")

add_subdirectory(libsbx-units)
add_subdirectory(libsbx-utility)
add_subdirectory(libsbx-io)
add_subdirectory(libsbx-memory)
add_subdirectory(libsbx-signals)
add_subdirectory(libsbx-containers)
add_subdirectory(libsbx-assets)
add_subdirectory(libsbx-bitmaps)
add_subdirectory(libsbx-math)
add_subdirectory(libsbx-ecs)
add_subdirectory(libsbx-core)
add_subdirectory(libsbx-devices)
add_subdirectory(libsbx-gizmos)
add_subdirectory(libsbx-graphics)
add_subdirectory(libsbx-scenes)
# add_subdirectory(libsbx-scripting)
add_subdirectory(libsbx-models)
add_subdirectory(libsbx-animations)
add_subdirectory(libsbx-shadows)
add_subdirectory(libsbx-post)
add_subdirectory(libsbx-ui)
add_subdirectory(libsbx-physics)
add_subdirectory(libsbx-editor)

if (${SBX_BUILD_DEMO})
  add_subdirectory(demo)
endif()
