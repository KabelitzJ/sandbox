project(demo)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_executable(${PROJECT_NAME})

find_program(POWERSHELL_PATH NAMES pwsh)

message(STATUS "POWERSHELL_PATH: ${POWERSHELL_PATH}")

add_custom_target(
  shaders
  COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.ps1" -directory "${PROJECT_SOURCE_DIR}/assets/shaders/"
)

add_dependencies(${PROJECT_NAME} shaders)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/demo.cpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    libsbx::units
    libsbx::utility
    libsbx::async
    libsbx::io
    libsbx::math
    libsbx::memory
    libsbx::core
    libsbx::devices
    libsbx::graphics
    libsbx::ecs
    libsbx::scenes
    libsbx::scripting
    libsbx::signal
    libsbx::models
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