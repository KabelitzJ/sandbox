#include "core.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "model_loader.hpp"

namespace sbx {

static GLFWwindow* _context = nullptr;
static bool _draw_wireframe = false;
static shader* _default_shader = nullptr;
static basic_model _default_model;
static GLuint _vao = 0;
static GLuint _vbo = 0;
static GLuint _ebo = 0;
static std::unordered_map<std::string, GLint> _uniform_map;
static glm::vec3 _uniform_color({ 0.0f, 0.0f, 0.0f });
static constexpr glm::vec3 _clear_color({ 0.33f, 0.45f, 0.50f });

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

  _context = glfwCreateWindow(920, 780, "Sandbox", nullptr, nullptr);

  glfwMakeContextCurrent(_context);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return false;
  }

  glfwSwapInterval(0);

  glViewport(0, 0, 960, 720);

  _initialize_window_callbacks();

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");

  _default_model = load_basic_model("resources/models/cube.obj");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _default_model.vertices.size(), _default_model.vertices.data(), GL_STATIC_DRAW);
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(0));
  glEnableVertexAttribArray(0);
  // color
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  // glEnableVertexAttribArray(1);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _default_model.indices.size(), _default_model.indices.data(), GL_STATIC_DRAW); 

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  std::srand(std::time(nullptr));

  return true;
}

void run() {
  std::chrono::nanoseconds frame_time(0);
  std::uint32_t frames = 0;
  std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  glm::mat4 projection = glm::mat4(1.0f);

  while (!glfwWindowShouldClose(_context)) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds passed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_time);
    last_time = now;

    frame_time += passed_time;

    if (frame_time >= std::chrono::seconds(1)) {
      std::cout << "FPS: " << frames << "\n";

      frame_time = std::chrono::nanoseconds(0);
      frames = 0;
    }

    float time_value = std::chrono::duration_cast<std::chrono::duration<float>>(passed_time).count();
    model = glm::rotate(model, time_value * glm::radians(50.0f), glm::vec3(1.0f, 0.5f, 0.0f));

    int width, height;
    glfwGetWindowSize(_context, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    GLint model_matrix_location = _get_uniform_location("uni_model");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));

    GLint view_matrix_location = _get_uniform_location("uni_view");
    glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, glm::value_ptr(view));

    GLint projection_matrix_location = _get_uniform_location("uni_projection");
    glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection));

    glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, _draw_wireframe ? GL_LINE : GL_FILL);

    _default_shader->bind();

    GLint color_location = _get_uniform_location("uni_color");
    glUniform4f(color_location, _uniform_color.r, _uniform_color.g, _uniform_color.b, 1.0f);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glDrawElements(GL_TRIANGLES, _default_model.indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glfwSwapBuffers(_context);

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
  glfwDestroyWindow(_context);
  glfwTerminate();
}

void _initialize_glfw_callbacks() {
  glfwSetErrorCallback([](int error_code, const char* description){
    std::cout << "[Error: " << error_code << "] " << description << "\n";
  });
}

void _initialize_window_callbacks() {
  glfwSetWindowSizeCallback(_context, [](GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
  });

  glfwSetKeyCallback(_context, [](GLFWwindow* window, int key, int scancode, int action, int mods){
    // close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(_context, true);
    }
    // toggle wireframe mode
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
      _draw_wireframe = !_draw_wireframe;
    }
    // randomize new uniform color
    if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _uniform_color.r = (static_cast<float>(std::rand() % 255) / 255);
      _uniform_color.g = (static_cast<float>(std::rand() % 255) / 255);
      _uniform_color.b = (static_cast<float>(std::rand() % 255) / 255);
    }
    // reset uniform color
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      _uniform_color = { 0.0f, 0.0f, 0.0f };
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
