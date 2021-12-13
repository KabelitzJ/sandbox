add_library(${CMAKE_PROJECT_NAME} INTERFACE)
add_library(${CMAKE_PROJECT_NAME}::${CMAKE_PROJECT_NAME} ALIAS ${CMAKE_PROJECT_NAME})

set(
  SBX_MODULES
    types
    math
    utils
    ecs
    core
    window
    rendering
    physics
)

foreach(module ${SBX_MODULES})
  add_subdirectory(${module})

  add_library(${CMAKE_PROJECT_NAME}::${module} ALIAS ${module})

  target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    INTERFACE
    ${CMAKE_PROJECT_NAME}::${module}
  )
endforeach()
