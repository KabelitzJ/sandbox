project(devices)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_library(${PROJECT_NAME} STATIC)
add_library(${CMAKE_PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/devices.cpp"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/devices.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/devices_module.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/events.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/window.hpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    # External dependencies
    fmt::fmt
    glfw
    # Internal dependencies
    libsbx::utility
    libsbx::units
    libsbx::core
    libsbx::signals
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
