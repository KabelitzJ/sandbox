if(${CMAKE_HOST_UNIX})
  set(_platform_specific_path "unix")
  message(STATUS "Using glad compiled for unix")
elseif(${CMAKE_HOST_WIN32})
  set(_platform_specific_path "win32")
  message(STATUS "Using glad compiled for win32")
else()
  message(FATAL_ERROR "Platform '${CMAKE_HOST_SYSTEM_NAME}' not supported!")
endif()

set(glad_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/glad/include")
set(glad_LIBRARY "${CMAKE_SOURCE_DIR}/externals/glad/lib/${_platform_specific_path}/libglad.a")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  glad
  FOUND_VAR glad_FOUND
  REQUIRED_VARS
    glad_LIBRARY
    glad_INCLUDE_DIR
)

if(glad_FOUND AND NOT TARGET glad::glad)
  add_library(glad::glad STATIC IMPORTED)

  set_target_properties(
    glad::glad 
    PROPERTIES
      IMPORTED_LOCATION "${glad_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${glad_INCLUDE_DIR}"
  )
endif()
