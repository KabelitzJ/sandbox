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

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "shader.hpp"
#include "mesh.hpp"

namespace sbx {

static GLFWwindow* _context = nullptr;
static bool _draw_wireframe = false;
static shader* _default_shader = nullptr;
static mesh* _default_mesh = nullptr;
static const std::vector<std::string> _texture_paths({
  "resources/textures/brick_wall.jpg",
  "resources/textures/cobble_wall.jpg",
  "resources/textures/lava.jpg",
  "resources/textures/wooden_box_komisch.jpg",
  "resources/textures/wooden_box.jpg",
  "resources/textures/wooden_planks.jpg",
});
static std::vector<GLuint> _textures;
static std::unordered_map<std::string, GLint> _uniform_map;
static constexpr glm::vec3 _clear_color({ 0.33f, 0.45f, 0.50f });
static unsigned int _texture_index = 2;

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

  GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();

  if (!primary_monitor) {
    std::cout << "[Error] Glfw could not detect a monitor!\n";

    return false;
  }

  const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);

  const int width = video_mode->width / 2;
  const int height = video_mode->height / 2;

  _context = glfwCreateWindow(width, height, "Sandbox", nullptr, nullptr);

  const int win_pos_x = (video_mode->width / 2) - (width / 2);
  const int win_pos_y = (video_mode->height / 2) - (height / 2);

  glfwSetWindowPos(_context, win_pos_x, win_pos_y);

  glfwMakeContextCurrent(_context);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return false;
  }

  glfwSwapInterval(0);

  glViewport(0, 0, width, height);

  _initialize_window_callbacks();

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");
  _default_shader->bind();

  _default_mesh = new mesh("resources/models/monke.obj");

  for (std::size_t i = 0; i < _texture_paths.size(); ++i) {

    GLuint id;

    glGenTextures(1, &id);

    _textures.push_back(id);

    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* texture_data = stbi_load(_texture_paths[i].c_str(), &width, &height, &nrChannels, 0);

    if (!texture_data) {
      std::cout << "[Error] Could not load default texture!\n";

      return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GLint texture_location = _get_uniform_location("uni_texture");
  glUniform1i(texture_location, _texture_index);

  GLint color_location = _get_uniform_location("uni_color");
  glUniform4f(color_location, 1.0f, 1.0f, 1.0f, 1.0f);

  glBindTexture(GL_TEXTURE_2D, 0);
  _default_shader->unbind();

  std::srand(std::time(nullptr));

  return true;
}

void run() {
  std::chrono::nanoseconds frame_time(0);
  std::uint32_t frames = 0;
  std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
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
    model = glm::rotate(model, time_value * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    int width, height;
    glfwGetWindowSize(_context, &width, &height);
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, _draw_wireframe ? GL_LINE : GL_FILL);

    _default_shader->bind();

    GLint model_matrix_location = _get_uniform_location("uni_model");
    glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model));

    GLint view_matrix_location = _get_uniform_location("uni_view");
    glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, glm::value_ptr(view));

    GLint projection_matrix_location = _get_uniform_location("uni_projection");
    glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, glm::value_ptr(projection));

    glBindTexture(GL_TEXTURE_2D, _textures[1]);
    
    _default_mesh->draw(*_default_shader);

    glfwSwapBuffers(_context);

    _default_shader->unbind();

    frames++;

    glfwPollEvents();
  }
}

void terminate() {
  delete _default_shader;
  delete _default_mesh;

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
      glm::vec3 color;

      color.r = (static_cast<float>(std::rand() % 255) / 255);
      color.g = (static_cast<float>(std::rand() % 255) / 255);
      color.b = (static_cast<float>(std::rand() % 255) / 255);

      _default_shader->bind();
      GLint color_location = _get_uniform_location("uni_color");
      glUniform4f(color_location, color.r, color.g, color.b, 1.0f);
      _default_shader->unbind();
    }
    // reset uniform color
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      _default_shader->bind();
      GLint color_location = _get_uniform_location("uni_color");
      glUniform4f(color_location, 1.0f, 1.0f, 1.0f, 1.0f);
      _default_shader->unbind();
    }
    // next texture
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
      _default_shader->bind();
      _texture_index = (_texture_index + 1) % _textures.size();
      GLint texture_location = _get_uniform_location("uni_texture");
      glUniform1i(texture_location, _texture_index);
      _default_shader->unbind();
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
