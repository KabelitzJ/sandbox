set(glfw3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/glfw3/include")
set(glfw3_LIBRARY "${CMAKE_SOURCE_DIR}/externals/glfw3/lib/win/libglfw3.a")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  glfw3
  FOUND_VAR glfw3_FOUND
  REQUIRED_VARS
    glfw3_LIBRARY
    glfw3_INCLUDE_DIR
  VERSION_VAR glfw3_VERSION
)

if(glfw3_FOUND AND NOT TARGET glfw3::glfw3)
  add_library(glfw3::glfw3 UNKNOWN IMPORTED)
  set_target_properties(glfw3::glfw3 PROPERTIES
    IMPORTED_LOCATION "${glfw3_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_glfw3_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${glfw3_INCLUDE_DIR}"
  )
endif()
