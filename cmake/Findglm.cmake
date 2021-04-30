set(glm_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/glm/include")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  glm
  FOUND_VAR glm_FOUND
  REQUIRED_VARS
    glm_INCLUDE_DIR
)

if(glm_FOUND AND NOT TARGET glm::glm)
  add_library(glm::glm UNKNOWN IMPORTED)
  set_target_properties(glm::glm PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${PC_glm_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${glm_INCLUDE_DIR}"
  )
endif()
