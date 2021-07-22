if(${CMAKE_HOST_UNIX})
  set(_platform_specific_path "unix")
  message(STATUS "Using glfw3 compiled for unix")
elseif(${CMAKE_HOST_WIN32})
  set(_platform_specific_path "win32")
  message(STATUS "Using glfw3 compiled for win32")
else()
  message(FATAL_ERROR "Platform '${CMAKE_HOST_SYSTEM_NAME}' not supported!")
endif()

set(glfw3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/glfw3/include")
set(glfw3_LIBRARY "${CMAKE_SOURCE_DIR}/externals/glfw3/lib/${_platform_specific_path}/libglfw3.a")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  glfw3
  FOUND_VAR glfw3_FOUND
  REQUIRED_VARS
    glfw3_LIBRARY
    glfw3_INCLUDE_DIR
)

if(glfw3_FOUND AND NOT TARGET glfw3::glfw3)
  add_library(glfw3::glfw3 STATIC IMPORTED)

  set_target_properties(
    glfw3::glfw3
    PROPERTIES
      IMPORTED_LOCATION "${glfw3_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${glfw3_INCLUDE_DIR}"
  )

  if(${CMAKE_HOST_UNIX})
    target_link_libraries(
      glfw3::glfw3
      INTERFACE
        pthread
        X11
        dl
    )
  endif()

endif()
