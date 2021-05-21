#include "core.hpp"

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <evtsys/event_queue.hpp>
#include <evtsys/key_event.hpp>
#include <evtsys/window_event.hpp>

#include "shader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "perspective_camera.hpp"

namespace sbx {

static GLFWwindow* _context = nullptr;
static event_queue* _event_queue;
static bool _draw_wireframe = false;
static camera* _camera = nullptr;

static shader* _default_shader = nullptr;
static mesh* _monke_mesh = nullptr;
static mesh* _floor_mesh = nullptr;
static mesh* _cube_mesh = nullptr;
static const std::filesystem::path _texture_dir("resources/textures");
static std::vector<texture*> _textures;
static unsigned int _active_texture_index = 0;
static constexpr glm::vec3 _clear_color({ 0.33f, 0.45f, 0.50f });

static glm::mat4 _model = glm::mat4(1.0f);
static glm::mat4 _view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
static glm::mat4 _projection = glm::mat4(1.0f);
static glm::vec3 _camera_position(0.0f, 0.0f, 4.0f);
static glm::vec3 _camera_direction(0.0f, 0.0f, -1.0f);
static constexpr glm::vec3 _up(0.0f, 1.0f, 0.0f);

static constexpr float _camera_speed = 0.25f;
static constexpr float _camera_sensitivity = 0.4f;
static float _camera_pitch = 0.0f;
static float _camera_yaw = -90.0f;
static float _fov = 45.0f;

static void _initialize_glfw_callbacks();

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

  if (!_context) {
    std::cout << "[Error] Glfw could not create a window!\n";

    return false;
  }

  glfwMakeContextCurrent(_context);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return false;
  }

  const int win_pos_x = (video_mode->width / 2) - (width / 2);
  const int win_pos_y = (video_mode->height / 2) - (height / 2);
  glfwSetWindowPos(_context, win_pos_x, win_pos_y);
  glfwSetInputMode(_context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(0);
  glfwFocusWindow(_context);

  _event_queue = new event_queue(_context);
  _event_queue->subscribe<key_pressed_event>([](key_pressed_event& event){
    if (event.code() == key_code::ESCAPE) {
      glfwSetWindowShouldClose(_context, true);
    }
  });

  float aspect = static_cast<float>(width) / static_cast<float>(height);

  _camera = new perspective_camera(_camera_position, _camera_direction, _camera_speed, _fov, aspect, 0.1f, 100.0f, _camera_pitch, _camera_yaw);
  _event_queue->register_listener(*_camera);

  glViewport(0, 0, width, height);

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");
  _default_shader->bind();

  _monke_mesh = new mesh("resources/models/monke.obj");
  _floor_mesh = new mesh("resources/models/floor.obj");
  _cube_mesh = new mesh("resources/models/cube.obj");

  for (const auto& file : std::filesystem::directory_iterator(_texture_dir)) {
    _textures.push_back(new texture(file.path()));
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  _default_shader->set_uniform_4f("uni_color", { 1.0f, 1.0f, 1.0f, 1.0f });

  glBindTexture(GL_TEXTURE_2D, 0);
  _default_shader->unbind();

  std::srand(std::time(nullptr));

  return true;
}

void run() {
  std::chrono::nanoseconds frame_time(0);
  std::uint32_t frames = 0;
  std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

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

    const float time_value = std::chrono::duration_cast<std::chrono::duration<float>>(passed_time).count();

    _event_queue->poll();

    _view = _camera->view();

    _model = glm::rotate(_model, time_value * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    _projection = _camera->projection();

    glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, _draw_wireframe ? GL_LINE : GL_FILL);

    _default_shader->bind();

    _default_shader->set_uniform_matrix_4fv("uni_model", _model);
    _default_shader->set_uniform_matrix_4fv("uni_view", _view);
    _default_shader->set_uniform_matrix_4fv("uni_projection", _projection);

    _default_shader->set_uniform_1i("uni_texture", _active_texture_index);
    _textures[_active_texture_index]->bind();
    
    _cube_mesh->draw(*_default_shader);

    glfwSwapBuffers(_context);

    _default_shader->unbind();

    frames++;
  }
}

void terminate() {
  delete _default_shader;
  delete _monke_mesh;
  delete _floor_mesh;
  delete _cube_mesh;

  for (texture* texture : _textures) {
    delete texture;
  }

  _textures.clear();

  delete _event_queue;

  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(_context);
  glfwTerminate();
}

void _initialize_glfw_callbacks() {
  glfwSetErrorCallback([](int error_code, const char* description){
    std::cout << "[Error: " << error_code << "] " << description << "\n";
  });
}

// void _initialize_window_callbacks() {
//   glfwSetFramebufferSizeCallback(_context, [](GLFWwindow* window, int width, int height){
//     glViewport(0, 0, width, height);
//   });

//   glfwSetKeyCallback(_context, [](GLFWwindow* window, int key, int scancode, int action, int mods){
//     // close window
//     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//       glfwSetWindowShouldClose(_context, true);
//     }
//     // toggle wireframe mode
//     if (key == GLFW_KEY_T && action == GLFW_PRESS) {
//       _draw_wireframe = !_draw_wireframe;
//     }
//     // randomize new uniform color
//     if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
//       glm::vec3 color;

//       color.r = (static_cast<float>(std::rand() % 255) / 255);
//       color.g = (static_cast<float>(std::rand() % 255) / 255);
//       color.b = (static_cast<float>(std::rand() % 255) / 255);

//       _default_shader->bind();
//       _default_shader->set_uniform_4f("uni_color", { color.r, color.g, color.b, 1.0f });
//       _default_shader->unbind();
//     }
//     // reset uniform color
//     if (key == GLFW_KEY_R && action == GLFW_PRESS) {
//       _default_shader->bind();
//       _default_shader->set_uniform_4f("uni_color", { 1.0f, 1.0f, 1.0f, 1.0f });
//       _default_shader->unbind();
//     }
//     // switch texture
//     if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
//       _active_texture_index = (_active_texture_index + 1) % _textures.size();
//     }
//   });

//   glfwSetCursorPosCallback(_context, [](GLFWwindow* window, double xpos, double ypos){
//     if (_is_first_cursor_movement) {
//       _last_cursor_position = glm::vec2(xpos, ypos);
//       _is_first_cursor_movement = false;
//     }

//     glm::vec2 cursor_offset(xpos - _last_cursor_position.x, _last_cursor_position.y - ypos);
//     cursor_offset *= _camera_sensitivity;

//     _last_cursor_position = glm::vec2(xpos, ypos);

//     _camera_yaw += cursor_offset.x;
//     _camera_pitch += cursor_offset.y;

//     if(_camera_pitch > 89.0f) {
//       _camera_pitch =  89.0f;
//     }
//     if(_camera_pitch < -89.0f) {
//       _camera_pitch = -89.0f;
//     }

//     glm::vec3 direction;
//     direction.x = cos(glm::radians(_camera_yaw)) * cos(glm::radians(_camera_pitch));
//     direction.y = sin(glm::radians(_camera_pitch));
//     direction.z = sin(glm::radians(_camera_yaw)) * cos(glm::radians(_camera_pitch));
//     _camera_target = glm::normalize(direction);
//   });

//   glfwSetScrollCallback(_context, [](GLFWwindow* window, double xoffset, double yoffset){
//     _fov -= static_cast<float>(yoffset) * _scroll_sensitivity;

//     if (_fov < 1.0f) {
//       _fov = 1.0f;
//     }
//     if (_fov > 45.0f) {
//       _fov = 45.0f;
//     }
//   });
// }

// void _process_input() {
//   if (glfwGetKey(_context, GLFW_KEY_W) == GLFW_PRESS) {
//     _camera_position += _camera_target * _camera_speed;
//   }
//   if (glfwGetKey(_context, GLFW_KEY_S) == GLFW_PRESS) {
//     _camera_position -= _camera_target * _camera_speed;
//   }
//   if (glfwGetKey(_context, GLFW_KEY_A) == GLFW_PRESS) {
//     _camera_position -= glm::normalize(glm::cross(_camera_target, _up)) * _camera_speed;
//   }
//   if (glfwGetKey(_context, GLFW_KEY_D) == GLFW_PRESS) {
//     _camera_position += glm::normalize(glm::cross(_camera_target, _up)) * _camera_speed;
//   }
//   if (glfwGetKey(_context, GLFW_KEY_Q) == GLFW_PRESS) {
//     _camera_position += _up * _camera_speed;
//   }
//   if (glfwGetKey(_context, GLFW_KEY_E) == GLFW_PRESS) {
//     _camera_position -= _up * _camera_speed;
//   }
// }

} // namespace sbx
