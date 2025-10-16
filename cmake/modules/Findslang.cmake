# Findslang.cmake
# Cross-platform find module for Slang (as bundled with the Vulkan SDK or standalone)
#
# Provides:
#   slang::slang  - imported target with include + library paths
#   slang_FOUND   - true if found
#   slang_INCLUDE_DIR
#   slang_LIBRARY
#   slang_DLL (Windows only)

# Typical layout:
#   Windows: $ENV{VULKAN_SDK}/Include/slang, /Lib/slang.lib, /Bin/slang.dll
#   Linux:   $ENV{VULKAN_SDK}/include/slang, /lib/libslang.so

# --- Search paths ----------------------------------------------------------

set(_SLANG_HINTS
  "$ENV{VULKAN_SDK}/Include/slang"
  "$ENV{VULKAN_SDK}/Include"
  "$ENV{VULKAN_SDK}/include/slang"
  "$ENV{VULKAN_SDK}/include"
  "/usr/include/slang"
  "/usr/local/include/slang"
)

find_path(slang_INCLUDE_DIR
  NAMES slang.h
  HINTS ${_SLANG_HINTS}
)

# Library names differ across platforms
if(WIN32)
  set(_SLANG_LIB_NAMES slang.lib slang)
else()
  set(_SLANG_LIB_NAMES slang libslang.so)
endif()

find_library(slang_LIBRARY
  NAMES ${_SLANG_LIB_NAMES}
  HINTS
    "$ENV{VULKAN_SDK}/Lib"
    "$ENV{VULKAN_SDK}/lib"
    "/usr/lib"
    "/usr/local/lib"
)

# Optional: runtime DLL for Windows
if(WIN32)
  find_file(slang_DLL
    NAMES slang.dll
    HINTS
      "$ENV{VULKAN_SDK}/Bin"
      "$ENV{VULKAN_SDK}/bin"
)
endif()

# --- Validation ------------------------------------------------------------

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(slang
  REQUIRED_VARS slang_INCLUDE_DIR slang_LIBRARY
  HANDLE_COMPONENTS
)

# --- Create imported target ------------------------------------------------

if(slang_FOUND AND NOT TARGET slang::slang)
  add_library(slang::slang SHARED IMPORTED GLOBAL)
  set_target_properties(slang::slang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${slang_INCLUDE_DIR}"
    IMPORTED_IMPLIB               "${slang_LIBRARY}"
    IMPORTED_LOCATION             "${slang_DLL}"
  )
endif()

mark_as_advanced(
  slang_INCLUDE_DIR
  slang_LIBRARY
  slang_DLL
)
