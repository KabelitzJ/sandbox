#include "core.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <ctime>

#include <glm/vec3.hpp>

#include "shader.hpp"

namespace sbx {

static GLFWwindow* _handle = nullptr;
static shader* _default_shader = nullptr;
static bool _draw_wireframe = false;
static GLuint _vao = 0;
static GLuint _vbo = 0;
static GLuint _ebo = 0;
static std::array<GLfloat, 12> _vertices({
   0.5f,  0.5f,  0.0f,  // top right
   0.5f, -0.5f,  0.0f,  // bottom right
  -0.5f, -0.5f,  0.0f,  // bottom left
  -0.5f,  0.5f,  0.0f   // top left 
});
static std::array<GLuint, 6> _indices({
  0, 1, 3,   // first triangle
  1, 2, 3    // second triangle
});
static std::unordered_map<std::string, GLint> _uniform_map;
static glm::vec3 _main_color({0.8f, 0.4f, 0.4f});

static void _initialize_glfw_callbacks();
static void _initialize_window_callbacks();
static GLint _get_uniform_location(const std::string& uniform_name);

bool initialize() {
  _initialize_glfw_callbacks();

  if (!glfwInit()) {
    std::cout << "[Error] Could not initialize glfw!\n";

    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _handle = glfwCreateWindow(920, 780, "Sandbox", nullptr, nullptr);

  glfwMakeContextCurrent(_handle);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return false;
  }

  glfwSwapInterval(0);

  glViewport(0, 0, 960, 720);

  _initialize_window_callbacks();

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), _indices.data(), GL_STATIC_DRAW); 

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");

  std::srand(std::time(nullptr));

  return true;
}

void run() {
  std::chrono::nanoseconds frame_time(0);
  std::uint32_t frames = 0;
  std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

  while (!glfwWindowShouldClose(_handle)) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds passed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_time);
    last_time = now;

    frame_time += passed_time;

    if (frame_time >= std::chrono::seconds(1)) {
      std::cout << "FPS: " << frames << "\n";

      frame_time = std::chrono::nanoseconds(0);
      frames = 0;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, _draw_wireframe ? GL_LINE : GL_FILL);

    _default_shader->bind();

    GLint uniform = _get_uniform_location("uni_color");

    glUniform4f(uniform, _main_color.r, _main_color.g, _main_color.b, 1.0f);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glfwSwapBuffers(_handle);

    frames++;

    glfwPollEvents();
  }
}

void terminate() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
  
  delete _default_shader;

  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(_handle);
  glfwTerminate();
}

void _initialize_glfw_callbacks() {
  glfwSetErrorCallback([](int error_code, const char* description){
    std::cout << "[Error: " << error_code << "] " << description << "\n";
  });
}

void _initialize_window_callbacks() {
  glfwSetWindowSizeCallback(_handle, [](GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
  });

  glfwSetKeyCallback(_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(_handle, true);
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
      _draw_wireframe = !_draw_wireframe;
    }
    if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _main_color.r = (static_cast<float>(std::rand() % 255) / 255);
      _main_color.g = (static_cast<float>(std::rand() % 255) / 255);
      _main_color.b = (static_cast<float>(std::rand() % 255) / 255);
    }
  });
}

static GLint _get_uniform_location(const std::string& uniform_name) {
  auto uniform = _uniform_map.find(uniform_name);

  if (uniform != _uniform_map.end()) {
    return uniform->second;
  }

  GLint location = glGetUniformLocation(_default_shader->id(), uniform_name.c_str());

  _uniform_map.emplace(uniform_name, location);

  return location;
}

} // namespace sbx
