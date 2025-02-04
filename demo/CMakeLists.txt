project(demo VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "Configuring ${PROJECT_NAME}...")

add_executable(${PROJECT_NAME})

if(WIN32)
  message(STATUS "Using WIN32 shell")

  find_program(POWERSHELL_PATH NAMES pwsh)

  message(STATUS "POWERSHELL_PATH: ${POWERSHELL_PATH}")

  add_custom_target(
    shaders
    COMMAND ${POWERSHELL_PATH} -ExecutionPolicy Bypass "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.ps1" -directory "${PROJECT_SOURCE_DIR}/assets/shaders/"
  )
elseif(UNIX)
  message(STATUS "Using UNIX shell")
  find_program(BASH_PATH NAMES bash)

  message(STATUS "BASH_PATH: ${BASH_PATH}")

  add_custom_target(
    shaders
    COMMAND ${BASH_PATH} "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.sh" "${PROJECT_SOURCE_DIR}/assets/shaders/"
  )
else()
  message(FATAL_ERROR "Unsupported platform detected")
endif()


add_dependencies(${PROJECT_NAME} shaders)

target_sources(
  ${PROJECT_NAME}
  PUBLIC
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/renderer.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/application.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/camera_controller.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/line.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/pipeline.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/vertex.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/terrain_subrenderer.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/terrain_module.hpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/voronoi.hpp"
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/renderer.cpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/application.cpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/line.cpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/demo.cpp"
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/terrain/vertex.cpp"
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    LIBSBX_IMGUI_DOCKING=$<IF:$<BOOL:${LIBSBX_IMGUI_DOCKING}>,1,0>
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
    libsbx::io
    libsbx::bitmaps
    libsbx::math
    libsbx::memory
    libsbx::signals
    libsbx::ecs
    libsbx::core
    libsbx::devices
    libsbx::gizmos
    libsbx::graphics
    libsbx::scenes
    libsbx::models
    libsbx::shadows
    libsbx::post
    libsbx::ui
    libsbx::physics
    libsbx::editor
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    SBX_CONSTEXPR_ENABLED=${SBX_CONSTEXPR_ENABLED}
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
