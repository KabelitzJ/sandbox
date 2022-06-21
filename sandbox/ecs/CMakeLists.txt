project(ecs CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    entity.cpp
    registry.cpp
  PUBLIC
    FILE_SET HEADERS 
    BASE_DIRS ${PROJECT_SOURCE_DIR}/..
    FILES 
      component_storage.hpp
      ecs.hpp
      entity_set.hpp
      entity.hpp
      registry.hpp
      registry.inl
      view.hpp
)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC 
    sbx::platform
    sbx::meta
    sbx::types
    sbx::utils
    sbx::math
)

target_compile_features(
  ${PROJECT_NAME}
  PUBLIC 
    cxx_std_20
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
    $<$<CXX_COMPILER_ID:GNU>:
      -Wall 
      -Wextra
      -Wshadow
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
    >
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/bin/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/lib/"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/lib/"
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${CMAKE_PROJECT_NAME}_${PROJECT_NAME}_export
  FILE_SET HEADERS
    DESTINATION "include/${CMAKE_PROJECT_NAME}"
  RUNTIME
    DESTINATION "bin"
  LIBRARY
    DESTINATION "lib"
  ARCHIVE
    DESTINATION "lib"
)

install(
  EXPORT ${CMAKE_PROJECT_NAME}_${PROJECT_NAME}_export
  DESTINATION "cmake"
  NAMESPACE ${PROJECT_NAME}::
)
