project(demo)

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
    libsbx::signals
    libsbx::assets
    libsbx::devices
    libsbx::graphics
    libsbx::ui
    libsbx::models
    libsbx::ecs
    libsbx::scenes
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    POSITION_INDEPENDENT_CODE ON
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

if(MINGW)
  target_link_options(
    ${PROJECT_NAME}
    PUBLIC
      -Wl,--disable-dynamicbase,--default-image-base-low
  )
endif()
