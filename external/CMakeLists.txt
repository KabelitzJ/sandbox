add_subdirectory(fmt)

add_subdirectory(glfw)

add_subdirectory(json)

option(SPDLOG_FMT_EXTERNAL "" ON)
add_subdirectory(spdlog)
