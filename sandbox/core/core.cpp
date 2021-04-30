#include "core.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include "file_utils.hpp"

namespace sbx {

static GLFWwindow* _handle = nullptr;
static GLuint _default_shader_program = 0;

static void initialize_glfw_callbacks();
static void initialize_window_callbacks();
static void initialize_shader_programs();

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

  initialize_shader_programs();
}

void run() {
  while (!glfwWindowShouldClose(_handle)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(_default_shader_program);

    glfwSwapBuffers(_handle);
  }
}

void terminate() {
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

void initialize_shader_programs() {
  const std::string vertext_shader_file = "resources/shaders/default_vertex.glsl";
  const std::string fragment_shader_file = "resources/shaders/default_fragment.glsl";

  std::string vertex_source = read_file_contents(vertext_shader_file);
  std::string fragment_source = read_file_contents(fragment_shader_file);

  const char* vertex_source_cstr = vertex_source.c_str();
  const char* fragment_source_cstr = fragment_source.c_str();

  int success = 0;
  char info_log[512];

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source_cstr, nullptr);
  glCompileShader(vertex_shader);

  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);

    std::stringstream ss;

    ss << "Could not compile vertex shader from file: '" << vertext_shader_file << "'!\n" 
       << "Log: " << info_log << "\n";

    throw std::runtime_error(ss.str());
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source_cstr, nullptr);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);

    std::stringstream ss;

    ss << "Could not compile fragment shader from file: '" << fragment_shader_file << "'!\n" 
       << "Log: " << info_log << "\n";

    throw std::runtime_error(ss.str());
  }

  _default_shader_program = glCreateProgram();
  glAttachShader(_default_shader_program, vertex_shader);
  glAttachShader(_default_shader_program, fragment_shader);
  glLinkProgram(_default_shader_program);

  glGetProgramiv(_default_shader_program, GL_LINK_STATUS, &success);

  if(!success) {
    glGetProgramInfoLog(_default_shader_program, 512, NULL, info_log);

    std::stringstream ss;

    ss << "Could not link shader program!\n"
       << "Log: " << info_log << "\n";
    
    throw std::runtime_error(ss.str());
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

} // namespace sbx
