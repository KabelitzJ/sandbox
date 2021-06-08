if(${CMAKE_HOST_UNIX})
  set(_platform_specific_path "unix")
  message(STATUS "Using freetype compiled for unix")
elseif(${CMAKE_HOST_WIN32})
  set(_platform_specific_path "win32")
  message(STATUS "Using freetype compiled for win32")
else()
  message(FATAL_ERROR "Platform '${CMAKE_HOST_SYSTEM_NAME}' not supported!")
endif()

set(freetype_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/freetype/include")
set(freetype_LIBRARY "${CMAKE_SOURCE_DIR}/externals/freetype/lib/${_platform_specific_path}/libfreetype.a")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  freetype
  FOUND_VAR freetype_FOUND
  REQUIRED_VARS
    freetype_LIBRARY
    freetype_INCLUDE_DIR
)

if(freetype_FOUND AND NOT TARGET freetype::freetype)
  add_library(freetype::freetype STATIC IMPORTED)

  set_target_properties(
    freetype::freetype 
    PROPERTIES
      IMPORTED_LOCATION "${freetype_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${freetype_INCLUDE_DIR}"
  )

  if(${CMAKE_HOST_UNIX})
    target_link_libraries(
      freetype::freetype
      INTERFACE
        png
        z
        harfbuzz
        brotlidec
    )
  endif()
endif()
