project(math)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_library(${PROJECT_NAME} STATIC)
add_library(${CMAKE_PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.7.0
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(yaml-cpp fmt nlohmann_json)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/math.cpp"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/math.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/concepts.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/matrix4x4.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/matrix4x4.ipp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector2.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector2.ipp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector3.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector3.ipp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector4.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/vector4.ipp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/color.hpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/angle.hpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    yaml-cpp
    fmt::fmt
    nlohmann_json
    libsbx::utility
    libsbx::core
    libsbx::io
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
