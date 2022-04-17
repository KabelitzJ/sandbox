# function(compile_shader _TARGET _SHADER _TYPE)
#   message(STATUS "${_TARGET} ${_SHADER} ${_TYPE}")

#   if(NOT _TARGET)
#     message(FATAL_ERROR "No target specified for shader compilation")
#   endif()

#   if(NOT _SHADER)
#     message(FATAL_ERROR "No shader specified for compilation")
#   endif()

#   if(NOT _TYPE)
#     message(FATAL_ERROR "No shader type specified for compilation")
#   endif()

#   if(${_TYPE} IN_LIST "vertex;fragment;geometry;tess_control;tess_evaluation;compute")
#     message(FATAL_ERROR "Invalid shader type specified for compilation")
#   endif()

#   find_package(Vulkan REQUIRED COMPONENTS glslc)
#   find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

#   if(NOT glslc_executable)
#     message(FATAL_ERROR "glslc could not be found")
#   endif()

#   get_filename_component(_TARGET_DIR ${_SHADER} PATH)
#   get_filename_component(_TARGET_FILE ${_SHADER} NAME_WE)

#   set(_OUTPUT_DIR ${_TARGET_DIR}/bin)
#   set(_OUTPUT_FILE ${_OUTPUT_DIR}/${_TARGET_FILE}.spv)

#   make_directory(${_OUTPUT_DIR})

#   add_custom_command(
#     COMMAND ${glslc_executable} -o ${_OUTPUT_FILE} -fshader-stage=${_TYPE} ${_SHADER}
#     OUTPUT ${_OUTPUT_FILE}
#     DEPENDS ${_SHADER}
#     COMMENT "Compiling ${_SHADER} to ${_OUTPUT_FILE}"
#   )

#   add_custom_target(${_TARGET_FILE}_target ALL DEPENDS ${_OUTPUT_FILE})

#   unset(_OUTPUT_DIR)
#   unset(_OUTPUT_FILE)
# endfunction()

function(compile_shader _TARGET _SHADER)
  if(NOT _TARGET)
    message(FATAL_ERROR "No target specified for shader compilation")
  endif()

  if(NOT _SHADER)
    message(FATAL_ERROR "No shader specified for compilation")
  endif()

  find_package(Vulkan REQUIRED COMPONENTS glslc)
  find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

  if(NOT glslc_executable)
    message(FATAL_ERROR "glslc could not be found")
  endif()

  get_filename_component(_SHADER_NAME ${_SHADER} NAME)

  message(STATUS "Compiling shader \'${_SHADER_NAME}\'")

  file(GLOB _SHADER_FILES ${_SHADER}/*.glsl)
  
  set(_OUTPUT_FILES)
  
  set(_OUTPUT_DIR ${_SHADER}/bin)

  if(NOT EXISTS ${_OUTPUT_DIR})
    make_directory(${_OUTPUT_DIR})
  endif()

  set(_SHADER_TYPES "vertex;fragment;geometry;tess_control;tess_evaluation;compute")

  foreach(_FILE IN ITEMS ${_SHADER_FILES})
    get_filename_component(_FILE_EXTENTION ${_FILE} LAST_EXT)

    if (NOT _FILE_EXTENTION OR NOT _FILE_EXTENTION STREQUAL ".glsl")
      message(FATAL_ERROR "File ${_FILE} uses invalid extension")
    endif()

    get_filename_component(_SHADER_TYPE ${_FILE} NAME_WE)

    if(${_SHADER_TYPE} IN_LIST "${_SHADER_TYPES}")
      message(STATUS "${_SHADER_TYPES}")
      message(FATAL_ERROR "Invalid shader type (${_SHADER_TYPE}) specified for compilation")
    endif()

    set(_OUTPUT_FILE ${_OUTPUT_DIR}/${_SHADER_TYPE}.spv)

    add_custom_command(
      COMMAND ${glslc_executable} -fshader-stage=${_SHADER_TYPE} -c ${_FILE} -o ${_OUTPUT_FILE}
      OUTPUT ${_OUTPUT_FILE}
      DEPENDS ${_FILE}
      COMMENT "Compiling ${_FILE} to ${_OUTPUT_FILE}"
    )

    list(APPEND _OUTPUT_FILES ${_OUTPUT_FILE})
  endforeach()

  add_custom_target(${_SHADER_NAME}_shader ALL DEPENDS ${_OUTPUT_FILES})
endfunction()
