set(glad_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/glad/include")
set(glad_LIBRARY "${CMAKE_SOURCE_DIR}/externals/glad/lib/win/libglad.a")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  glad
  FOUND_VAR glad_FOUND
  REQUIRED_VARS
    glad_LIBRARY
    glad_INCLUDE_DIR
  VERSION_VAR glad_VERSION
)

if(glad_FOUND AND NOT TARGET glad::glad)
  add_library(glad::glad UNKNOWN IMPORTED)
  set_target_properties(glad::glad PROPERTIES
    IMPORTED_LOCATION "${glad_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_glad_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${glad_INCLUDE_DIR}"
  )
endif()
