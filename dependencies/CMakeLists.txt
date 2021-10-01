include(FetchContent)

# Including glfw3

FetchContent_Declare(
  glfw3
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.3.4"
)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

# Including glad

FetchContent_Declare(
  glad
  GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
  GIT_TAG "v0.1.34"
)

set(GLAD_API "gl=3.3")
set(GLAD_PROFILE "core")

# Including glm

FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG "0.9.9.8"
)

set(GLM_TEST_ENABLE OFF)

FetchContent_MakeAvailable(
  glfw3
  glad
  glm
)
