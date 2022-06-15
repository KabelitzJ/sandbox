include(compile_shader)

project(demo CXX)

find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME})

compile_shader(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/basic
)

add_dependencies(
  ${PROJECT_NAME}
  sbx::sbx
  Vulkan::Vulkan
  glfw
  spdlog
  basic_shader
)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    key.cpp
    button.cpp
    demo.cpp
  PUBLIC
    FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}/..
    FILES 
      application.hpp
      enumerate.hpp
      logger.hpp
      pipeline.hpp
      callback_container.hpp
      subscription.hpp
      event_manager.hpp
      instance.hpp
      window.hpp
      monitor.hpp
      physical_device.hpp
      logical_device.hpp
      surface.hpp
      swapchain.hpp
      key.hpp
      button.hpp
      modifiers.hpp
      render_pass.hpp
      framebuffers.hpp
      command_pool.hpp
      command_buffer.hpp
      semaphore.hpp
      fence.hpp
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    sbx::sbx
    Vulkan::Vulkan
    glfw
    spdlog
)

target_compile_features(
  ${PROJECT_NAME}
  PUBLIC 
    cxx_std_20
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
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

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    GLFW_INCLUDE_VULKAN
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/bin/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/lib/"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/lib/"
)
