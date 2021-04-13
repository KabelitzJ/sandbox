cmake_minimum_required(VERSION 3.15)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

project(sandbox CXX)

add_subdirectory(thread_pool)
add_subdirectory(object_pool)

add_executable(${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PRIVATE
    sandbox.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
    thread_pool::thread_pool
    object_pool::object_pool
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
