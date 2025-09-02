# FindMono.cmake — provides mono::mono + MONO_* vars
include(FindPackageHandleStandardArgs)

set(MONO_INCLUDE_DIRS "")
set(MONO_LIBRARIES "")
set(MONO_VERSION "")

# --- Prefer pkg-config for includes/version -----------------------------------
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(_MONO_PC QUIET mono-2)
  if(_MONO_PC_FOUND)
    set(MONO_INCLUDE_DIRS ${_MONO_PC_INCLUDE_DIRS})
    set(MONO_VERSION      ${_MONO_PC_VERSION})
  endif()
endif()

# --- Fallback/include discovery ----------------------------------------------
if(NOT MONO_INCLUDE_DIRS)
  find_path(MONO_INCLUDE_DIRS
    NAMES mono/jit/jit.h mono/metadata/assembly.h
    PATHS
      /usr/include /usr/local/include
      /opt/homebrew/include /usr/local/opt/mono/include
      "C:/Program Files/Mono/include" "C:/Program Files (x86)/Mono/include"
    PATH_SUFFIXES mono-2.0
  )
endif()

# --- Hard-require the actual runtime library (monosgen) -----------------------
# We DO NOT trust pkg-config's Libs line; we resolve monosgen ourselves.
find_library(MONO_LIBRARY
  NAMES monosgen-2.0 mono-2.0
  PATHS
    /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
    /opt/homebrew/lib /usr/local/opt/mono/lib
    "C:/Program Files/Mono/lib" "C:/Program Files (x86)/Mono/lib"
)

if(MONO_LIBRARY)
  set(MONO_LIBRARIES "${MONO_LIBRARY}")
endif()

# Try to derive version from headers if pkg-config didn't give one
if(NOT MONO_VERSION AND MONO_INCLUDE_DIRS)
  find_path(_MONO_VER_DIR NAMES version.h
    PATHS ${MONO_INCLUDE_DIRS}
    PATH_SUFFIXES mono mono-2.0/mono
  )
  if(_MONO_VER_DIR AND EXISTS "${_MONO_VER_DIR}/version.h")
    file(STRINGS "${_MONO_VER_DIR}/version.h" _vers REGEX "#define[ \t]+MONO_VERSION")
    if(_vers MATCHES "\"([0-9]+\\.[0-9]+\\.[0-9]+)\"")
      set(MONO_VERSION "${CMAKE_MATCH_1}")
    endif()
  endif()
endif()

find_package_handle_standard_args(Mono
  REQUIRED_VARS MONO_INCLUDE_DIRS MONO_LIBRARIES
  VERSION_VAR MONO_VERSION
)

if(MONO_FOUND AND NOT TARGET mono::mono)
  add_library(mono::mono UNKNOWN IMPORTED)
  set_target_properties(mono::mono PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MONO_INCLUDE_DIRS}"
    IMPORTED_LOCATION             "${MONO_LIBRARY}"
  )
  # If you *really* want to add system libs explicitly, uncomment:
  # if(UNIX AND NOT APPLE)
  #   set_property(TARGET mono::mono APPEND PROPERTY INTERFACE_LINK_LIBRARIES m dl pthread rt)
  # endif()
endif()
