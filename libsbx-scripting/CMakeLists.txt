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
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/memory.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/string.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/garbage_collection.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/assembly.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/attribute.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/type.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/field_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/property_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/method_info.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/object.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/runtime.cpp"
    "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/detail/type_cache.cpp"
  PUBLIC
    FILE_SET HEADERS
    FILES
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripting_module.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/scripts.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/fwd.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/platform.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/core.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/stable_vector.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/memory.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/string.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/garbage_collection.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/assembly.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/attribute.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/type.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/field_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/property_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/method_info.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/object.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/message_type.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/runtime.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/detail/type_cache.hpp"
      "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/detail/backend.hpp"
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

set(SBX_MANAGED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dotnet/Sbx.Managed)
set(SBX_MANAGED_PROJECT ${SBX_MANAGED_DIR}/Sbx.Managed.csproj)
file(GLOB_RECURSE SBX_MANAGED_SOURCES CONFIGURE_DEPENDS ${SBX_MANAGED_DIR}/*.cs ${SBX_MANAGED_DIR}/Interop/*.cs)
set(NATIVE_OUT_DIR ${CMAKE_BINARY_DIR}/_native_out)
set(DOTNET_OUT_DIR ${CMAKE_BINARY_DIR}/_dotnet_out)
set(DOTNET_CFG_OUT_DIR ${DOTNET_OUT_DIR}/$<CONFIG>)
set(NATIVE_CFG_OUT_DIR ${NATIVE_OUT_DIR}/$<CONFIG>)
set(DOTNET_STAMP ${DOTNET_OUT_DIR}/build.timestamp)
set(NATIVE_STAMP ${NATIVE_OUT_DIR}/native.ready)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${NATIVE_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${NATIVE_OUT_DIR}/
  COMMAND ${CMAKE_COMMAND} -E touch ${NATIVE_STAMP}
  COMMENT "Exporting ${PROJECT_NAME} to ${NATIVE_OUT_DIR} and writing stamp"
)

add_custom_command(
  OUTPUT ${DOTNET_STAMP}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOTNET_CFG_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E env "NativeOutputDir=${NATIVE_CFG_OUT_DIR}" dotnet build ${SBX_MANAGED_PROJECT} -c $<CONFIG> -o ${DOTNET_CFG_OUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${DOTNET_STAMP}
  DEPENDS
    ${SBX_MANAGED_PROJECT}
    ${SBX_MANAGED_SOURCES}
    ${NATIVE_STAMP}
  BYPRODUCTS
    ${DOTNET_CFG_OUT_DIR}/Sbx.Managed.dll
  COMMENT "Building .NET interop library (Sbx.Managed) for $<CONFIG>"
  VERBATIM
)

add_custom_target(managed ALL DEPENDS ${DOTNET_STAMP})

add_dependencies(managed ${PROJECT_NAME})

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


