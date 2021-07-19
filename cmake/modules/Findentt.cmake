set(entt_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/entt/include")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  entt
  FOUND_VAR entt_FOUND
  REQUIRED_VARS
    entt_INCLUDE_DIR
)

if(entt_FOUND AND NOT TARGET entt::entt)
  add_library(entt::entt INTERFACE IMPORTED)
  
  set_target_properties(
    entt::entt 
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${entt_INCLUDE_DIR}"
  )
endif()
