project(core)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_library(${PROJECT_NAME} STATIC)
add_library(${CMAKE_PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.11.0
)

FetchContent_MakeAvailable(fmt spdlog)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/core.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/application.cpp"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/core.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/application.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/assert.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/concepts.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/delegate.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/engine.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/logger.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/module.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/target.hpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    fmt::fmt
    spdlog::spdlog
    libsbx::utility
    libsbx::memory
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_features(
  ${PROJECT_NAME}
  PUBLIC
    cxx_std_20
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
    -Wall 
    -Wextra
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wuseless-cast
    -static-libstdc++ 
    -static-libgcc
)