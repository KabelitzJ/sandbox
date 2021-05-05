#include "core.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include "shader.hpp"

namespace sbx {

static GLFWwindow* _handle = nullptr;
static GLuint _default_shader_program = 0;
static shader* _default_shader;

static void initialize_glfw_callbacks();
static void initialize_window_callbacks();

void initialize() {
  initialize_glfw_callbacks();

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _handle = glfwCreateWindow(960, 720, "core", nullptr, nullptr);

  glfwMakeContextCurrent(_handle);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glfwSwapInterval(0);

  glViewport(0, 0, 960, 720);

  glClearColor(1.0f, 0.5f, 0.125f, 1.0f);

  initialize_window_callbacks();

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");
}

void run() {
  while (!glfwWindowShouldClose(_handle)) {
    glfwPollEvents();

    _default_shader->bind();

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(_default_shader_program);

    glfwSwapBuffers(_handle);

    _default_shader->unbind();
  }
}

void terminate() {
  delete _default_shader;
  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(_handle);
  glfwTerminate();
}

void initialize_glfw_callbacks() {
  glfwSetErrorCallback([](int error_code, const char* description){
    std::cout << "[Error: " << error_code << "] " << description << "\n";
  });
}

void initialize_window_callbacks() {
  glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
  });
}

} // namespace sbx
