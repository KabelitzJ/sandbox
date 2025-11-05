project(scripting VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "Configuring ${PROJECT_NAME}...")

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

configure_file("${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/version.hpp.in" "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/version.hpp" @ONLY)

if(${SBX_BUILD_SHARED})
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

add_library(${CMAKE_PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# find_package(Mono REQUIRED)
find_package(fmt REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(sol2 REQUIRED)
find_package(Nethost REQUIRED)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting_module.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripts.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/interop.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/memory.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/string.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/garbage_collection.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/assembly.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/attribute.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/type.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/field_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/property_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/method_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/object.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/runtime.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/detail/type_cache.cpp"
  PUBLIC
    FILE_SET HEADERS
    FILES
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting_module.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripts.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/interop.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/fwd.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/platform.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/core.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/stable_vector.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/memory.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/string.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/garbage_collection.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/assembly.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/attribute.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/type.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/field_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/property_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/method_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/object.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/message_type.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/runtime.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/detail/type_cache.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/managed/detail/backend.hpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    # External dependencies
    # mono::mono
    sol2::sol2
    fmt::fmt
    yaml-cpp
    Nethost::nethost
    # Internal dependencies
    libsbx::utility
    libsbx::core
    libsbx::scenes
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
    SBX_CONSTEXPR_ENABLED=${SBX_CONSTEXPR_ENABLED}
)

target_compile_features(
  ${PROJECT_NAME}
  PUBLIC
    cxx_std_23
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
    -Wall 
    -Wextra
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wuseless-cast
)

set(DOTNET_OUT_DIR ${CMAKE_BINARY_DIR}/_dotnet_out)

set(SBX_MANAGED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dotnet/Sbx.Managed)
set(SBX_MANAGED_PROJECT ${SBX_MANAGED_DIR}/Sbx.Managed.csproj)
set(SBX_MANAGED_STAMP ${SBX_MANAGED_DIR}/build.timestamp)
file(GLOB_RECURSE SBX_MANAGED_SOURCES CONFIGURE_DEPENDS ${SBX_MANAGED_DIR}/*.cs ${SBX_MANAGED_DIR}/Interop/*.cs)

add_custom_command(
  OUTPUT ${SBX_MANAGED_STAMP}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOTNET_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E env dotnet build ${SBX_MANAGED_PROJECT} -c $<CONFIG> -o ${DOTNET_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${SBX_MANAGED_STAMP}
  DEPENDS
    ${SBX_MANAGED_PROJECT}
    ${SBX_MANAGED_SOURCES}
  BYPRODUCTS
    ${DOTNET_OUT_DIR}/Sbx.Managed.dll
  COMMENT "Building .NET interop library (Sbx.Managed) for $<CONFIG>"
  VERBATIM
)

add_custom_target(scripting_managed ALL DEPENDS ${SBX_MANAGED_STAMP})

add_dependencies(scripting_managed ${PROJECT_NAME})

set(SBX_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dotnet/Sbx.Core)
set(SBX_CORE_PROJECT ${SBX_CORE_DIR}/Sbx.Core.csproj)
set(SBX_CORE_STAMP ${SBX_CORE_DIR}/build.timestamp)
file(GLOB_RECURSE SBX_CORE_SOURCES CONFIGURE_DEPENDS ${SBX_CORE_DIR}/*.cs)

add_custom_command(
  OUTPUT ${SBX_CORE_STAMP}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOTNET_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E env dotnet build ${SBX_CORE_PROJECT} -c $<CONFIG> -o ${DOTNET_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${SBX_CORE_STAMP}
  DEPENDS
    ${SBX_CORE_PROJECT}
    ${SBX_CORE_SOURCES}
    ${SBX_MANAGED_STAMP}
  BYPRODUCTS
    ${DOTNET_OUT_DIR}/Sbx.Core.dll
  COMMENT "Building .NET core library (Sbx.Core) for $<CONFIG>"
  VERBATIM
)

add_custom_target(scripting_core ALL DEPENDS ${SBX_CORE_STAMP})

add_dependencies(scripting_core ${PROJECT_NAME})

install(
  TARGETS
    ${PROJECT_NAME}
  EXPORT
    ${PROJECT_NAME}Targets
  LIBRARY 
    DESTINATION lib
  ARCHIVE 
    DESTINATION lib
  RUNTIME 
    DESTINATION bin
  FILE_SET 
    HEADERS 
)

set(_LINK_OPTIONS)

# if(NOT MINGW)
#   list(APPEND _LINK_OPTIONS -fsanitize=address,undefined)
# endif()

if(MINGW)
  list(APPEND _LINK_OPTIONS -Wl,--disable-dynamicbase,--default-image-base-low)
endif()

target_link_options(
  ${PROJECT_NAME}
  PUBLIC
    ${_LINK_OPTIONS}
)


