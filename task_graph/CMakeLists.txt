project(task_graph)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(
  ${PROJECT_NAME}
  INTERFACE
    thread_pool::thread_pool
    $<$<PLATFORM_ID:"Linux">:"tbb pthread">
)

target_compile_features(
  ${PROJECT_NAME} 
  INTERFACE 
    cxx_std_17
)

target_compile_options(
  ${PROJECT_NAME} 
  INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
)