project(demo CXX)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    demo.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    sbx::sbx
)

target_compile_features(
  ${PROJECT_NAME}
  PRIVATE 
    cxx_std_17
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>"
)
