project(math CXX)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME} STATIC)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    vector2.cpp
    vector3.cpp
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    sbx::types
    glm::glm
    spdlog::spdlog
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE 
    cxx_std_17
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
      -Wall 
      -Wextra
      -Wshadow
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
    >
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
)
