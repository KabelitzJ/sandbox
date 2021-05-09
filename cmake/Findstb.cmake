set(stb_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/stb/include")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  stb
  FOUND_VAR stb_FOUND
  REQUIRED_VARS
    stb_INCLUDE_DIR
)

if(stb_FOUND AND NOT TARGET stb::stb)
  add_library(stb::stb INTERFACE IMPORTED)
  
  set_target_properties(
    stb::stb 
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${stb_INCLUDE_DIR}"
  )
endif()
