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
  
  set(_OUTPUT_FILES)
  
  set(_OUTPUT_DIR ${_SHADER}/bin)

  message(STATUS "Output directory for shader \'${_SHADER_NAME}\' is \'${_OUTPUT_DIR}\'")

  if(NOT EXISTS ${_OUTPUT_DIR})
    make_directory(${_OUTPUT_DIR})
  endif()

  set(_SHADER_TYPES "vertex;fragment;geometry;tess_control;tess_evaluation;compute")

  foreach(_TYPE IN ITEMS ${_SHADER_TYPES})
    set(_FILE ${_SHADER}/${_TYPE}.glsl)

    if(NOT EXISTS ${_FILE})
      message(STATUS "Shader \'${_SHADER_NAME}\' has no \'${_TYPE}\' stage. Skipping.")
      continue()
    endif()

    get_filename_component(_FILE_EXTENTION ${_FILE} LAST_EXT)

    if (NOT _FILE_EXTENTION OR NOT _FILE_EXTENTION STREQUAL ".glsl")
      message(FATAL_ERROR "File ${_FILE} uses invalid extension")
    endif()

    set(_OUTPUT_FILE ${_OUTPUT_DIR}/${_TYPE}.spv)

    add_custom_command(
      COMMAND ${glslc_executable} -fshader-stage=${_TYPE} -c ${_FILE} -o ${_OUTPUT_FILE}
      OUTPUT ${_OUTPUT_FILE}
      DEPENDS ${_FILE}
      COMMENT "Compiling ${_FILE} to ${_OUTPUT_FILE}"
    )

    list(APPEND _OUTPUT_FILES ${_OUTPUT_FILE})
  endforeach()

  add_custom_target(${_SHADER_NAME}_shader DEPENDS ${_OUTPUT_FILES})
endfunction()
