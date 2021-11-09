project(core CXX)

find_package(spdlog REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(${PROJECT_NAME} STATIC)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    entry_point.cpp
    engine.cpp
    module.cpp
    scene.cpp
    uuid.cpp
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
  PUBLIC
    sbx::ecs
    sbx::types
    spdlog::spdlog
    glfw::glfw
    glm::glm
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE 
    cxx_std_17
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    GLFW_INCLUDE_NONE
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
