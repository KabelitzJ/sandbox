project(core CXX)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(${PROJECT_NAME} SHARED)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    core.cpp
    file_utils.cpp
    shader.cpp
    model_loader.cpp
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
    glad::glad
    glfw3::glfw3
    glm::glm
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE 
    cxx_std_17
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
)
