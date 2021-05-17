add_library(${CMAKE_PROJECT_NAME} INTERFACE)
add_library(${CMAKE_PROJECT_NAME}::${CMAKE_PROJECT_NAME} ALIAS ${CMAKE_PROJECT_NAME})

set(_SBX_LIBS evtsys core)

foreach(lib ${_SBX_LIBS})
  add_subdirectory(${lib})

  target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    INTERFACE
    ${CMAKE_PROJECT_NAME}::${lib}
  )
endforeach()
