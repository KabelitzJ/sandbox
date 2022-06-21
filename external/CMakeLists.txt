message(STATUS "Starting...")

add_subdirectory(fmt)

add_subdirectory(glfw)

if(NOT TARGET glfw::glfw3)
  add_library(glfw::glfw3 ALIAS glfw)
endif()

add_subdirectory(json)

add_subdirectory(spdlog)
