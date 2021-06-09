#ifndef SBX_CORE_HPP_
#define SBX_CORE_HPP_

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <ctime>
#include <thread>
#include <future>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#include "input_manager.hpp"
#include "object.hpp"

namespace sbx {

class engine {

public:
  engine() = default;
  ~engine() = default;

  void start();

private:

  void _initialize();

  void _run();

  void _terminate();

  void render_text(GLuint VAO, GLuint VBO, const std::string& text, float x, float y, glm::vec3 color = { 1.0f, 1.0f, 1.0f}, float scale = 1.0f);

  GLFWwindow* _context = nullptr;
  event_queue* _event_queue = nullptr;
  input_manager* _input = nullptr;
  bool _draw_wireframe = false;
  camera* _camera = nullptr;
  std::vector<object*> _objects;
  shader* _default_shader = nullptr;
  shader* _lighting_scene_shader = nullptr;
  shader* _lighting_source_shader = nullptr;
  shader* _text_shader = nullptr;
  std::unordered_map<std::string, mesh*> _mesh_atlas;
  std::unordered_map<std::string, texture*> _texture_atlas;
  glm::vec3 _clear_color = { 0.20f, 0.20f, 0.20f };
  glm::vec3 _camera_position = { 0.0f, 0.0f, 10.0f };
  glm::vec3 _camera_direction = { 0.0f, 0.0f, -1.0f };
  glm::vec3 _up = { 0.0f, 1.0f, 0.0f };
  float _camera_speed = 10.0f;
  float _camera_sensitivity = 0.4f;
  float _camera_pitch = 0.0f;
  float _camera_yaw = -90.0f;
  float _fov = 45.0f;
  glm::mat4 _gui_projection = glm::mat4(1.0f);

}; // class engine

} // namespace sbx

#endif // SBX_CORE_HPP_
