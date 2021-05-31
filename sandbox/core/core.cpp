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
#include <thread>
#include <future>

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

static GLFWwindow* _context = nullptr;
static event_queue* _event_queue = nullptr;
static input_manager* _input = nullptr;
static bool _draw_wireframe = false;
static camera* _camera = nullptr;
static std::vector<object*> _objects;

static shader* _default_shader = nullptr;
static shader* _lighting_scene_shader = nullptr;
static shader* _lighting_source_shader = nullptr;
static std::unordered_map<std::string, mesh*> _mesh_atlas;
static std::unordered_map<std::string, texture*> _texture_atlas;
// static constexpr glm::vec3 _clear_color({ 0.33f, 0.45f, 0.50f });
static constexpr glm::vec3 _clear_color({ 0.20f, 0.20f, 0.20f });

static glm::vec3 _camera_position(0.0f, 0.0f, 10.0f);
static glm::vec3 _camera_direction(0.0f, 0.0f, -1.0f);
static constexpr glm::vec3 _up(0.0f, 1.0f, 0.0f);

static constexpr float _camera_speed = 10.0f;
static constexpr float _camera_sensitivity = 0.4f;
static float _camera_pitch = 0.0f;
static float _camera_yaw = -90.0f;
static float _fov = 45.0f;

static void _initialize_glfw_callbacks();

template<typename T, typename... Args>
static T* _load_async(Args&&... args);

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

  const int width = video_mode->width;
  const int height = video_mode->height;

  _context = glfwCreateWindow(width, height, "Sandbox", primary_monitor, nullptr);

  if (!_context) {
    std::cout << "[Error] Glfw could not create a window!\n";

    return false;
  }

  glfwMakeContextCurrent(_context);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return false;
  }

  glfwSetInputMode(_context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(1);
  glfwFocusWindow(_context);

  _event_queue = new event_queue(_context);
  _event_queue->subscribe<key_pressed_event>([](key_pressed_event& event){
    if (event.code == key_code::ESCAPE) {
      glfwSetWindowShouldClose(_context, true);
    }
  });
  _event_queue->subscribe<key_pressed_event>([](key_pressed_event& event){
    if (event.code == key_code::ENTER) {
      _draw_wireframe = !_draw_wireframe;
    }
  });

  _input = new input_manager(*_event_queue);

  float aspect = static_cast<float>(width) / static_cast<float>(height);

  _camera = new perspective_camera(_camera_position, _camera_direction, _camera_speed, _fov, aspect, 0.1f, 100.0f, _camera_pitch, _camera_yaw);

  glViewport(0, 0, width, height);

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");
  _lighting_source_shader = new shader("resources/shaders/lighting_source_vertex.glsl", "resources/shaders/lighting_source_fragment.glsl");
  _lighting_scene_shader = new shader("resources/shaders/lighting_scene_vertex.glsl", "resources/shaders/lighting_scene_fragment.glsl");

  _lighting_scene_shader->bind();

  /* Setting up static shader uniforms */
  _lighting_scene_shader->set_uniform_1f("uni_material.shininess", 32.0f);
  _lighting_scene_shader->set_uniform_3f("uni_light.position", { 0.0f, 0.0f, 0.0f });
  _lighting_scene_shader->set_uniform_3f("uni_light.ambient", { 0.2f, 0.2f, 0.2f });
  _lighting_scene_shader->set_uniform_3f("uni_light.diffuse", { 0.5f, 0.5f, 0.5f });
  _lighting_scene_shader->set_uniform_3f("uni_light.specular", { 1.0f, 1.0f, 1.0f });

  /* loading meshes */
  _mesh_atlas.emplace("barrel", _load_async<mesh>("resources/models/barrel.obj"));
  _mesh_atlas.emplace("big_f", _load_async<mesh>("resources/models/big_f.obj"));
  _mesh_atlas.emplace("cone", _load_async<mesh>("resources/models/cone.obj"));
  _mesh_atlas.emplace("cube", _load_async<mesh>("resources/models/cube.obj"));
  _mesh_atlas.emplace("cylinder", _load_async<mesh>("resources/models/cylinder.obj"));
  _mesh_atlas.emplace("floor", _load_async<mesh>("resources/models/floor.obj"));
  _mesh_atlas.emplace("male", _load_async<mesh>("resources/models/male.obj"));
  _mesh_atlas.emplace("monke", _load_async<mesh>("resources/models/monke.obj"));
  _mesh_atlas.emplace("plane", _load_async<mesh>("resources/models/plane.obj"));
  _mesh_atlas.emplace("sphere", _load_async<mesh>("resources/models/sphere.obj"));
  _mesh_atlas.emplace("torus", _load_async<mesh>("resources/models/torus.obj"));
  _mesh_atlas.emplace("wooden_box", _load_async<mesh>("resources/models/wooden_box.obj"));
  _mesh_atlas.emplace("smg", _load_async<mesh>("resources/models/smg.obj"));

  /* loading textures */
  _texture_atlas.emplace("blank", _load_async<texture>("resources/textures/blank.jpg"));
  _texture_atlas.emplace("filled", _load_async<texture>("resources/textures/filled.jpg"));
  _texture_atlas.emplace("brick_wall", _load_async<texture>("resources/textures/brick_wall.jpg"));
  _texture_atlas.emplace("cobble_wall", _load_async<texture>("resources/textures/cobble_wall.jpg"));
  _texture_atlas.emplace("lava", _load_async<texture>("resources/textures/lava.jpg"));
  _texture_atlas.emplace("wooden_planks", _load_async<texture>("resources/textures/wooden_planks.jpg"));
  _texture_atlas.emplace("barrel.diffusion", _load_async<texture>("resources/textures/barrel/diffusion.png"));
  _texture_atlas.emplace("barrel.normal", _load_async<texture>("resources/textures/barrel/normal.png"));
  _texture_atlas.emplace("barrel.specular", _load_async<texture>("resources/textures/barrel/specular.png"));
  _texture_atlas.emplace("wooden_box", _load_async<texture>("resources/textures/wooden_box.png"));
  _texture_atlas.emplace("smg.diffusion", _load_async<texture>("resources/textures/smg/diffusion.png"));
  _texture_atlas.emplace("smg.S", _load_async<texture>("resources/textures/smg/S.png"));
  _texture_atlas.emplace("smg.normal", _load_async<texture>("resources/textures/smg/normal.png"));
  _texture_atlas.emplace("smg.specular", _load_async<texture>("resources/textures/smg/specular.png"));
  _texture_atlas.emplace("wooden_box_steel_border", _load_async<texture>("resources/textures/wooden_box_steel_border.png"));
  _texture_atlas.emplace("wooden_box_steel_border_specular", _load_async<texture>("resources/textures/wooden_box_steel_border_specular.png"));
  _texture_atlas.emplace("random_specular", _load_async<texture>("resources/textures/random_specular.jpg"));

  // This one is the light source
  _objects.push_back(new object(
    *_mesh_atlas["sphere"],
    {
      _texture_atlas["blank"],
      _texture_atlas["blank"],
    },
    {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.25f, 0.25f, 0.25f),
    }
  ));

  // this one is the floor
  _objects.push_back(new object(
    *_mesh_atlas["plane"],
    {
      _texture_atlas["blank"],
      _texture_atlas["filled"],
    },
    {
      glm::vec3(0.0f, -3.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.5f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["torus"],
    {
      _texture_atlas["lava"],
      _texture_atlas["blank"],
    },
    {
      glm::vec3(5.0f, 2.0f, 0.0f),
      glm::vec3(45.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["monke"],
    {
      _texture_atlas["wooden_planks"],
      _texture_atlas["blank"],
    },
    {
      glm::vec3(-3.0f, 0.0f, 1.0f),
      glm::vec3(0.0f, 45.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["cube"],
    {
      _texture_atlas["wooden_box_steel_border"],
      _texture_atlas["wooden_box_steel_border_specular"],
    },
    {
      glm::vec3(3.0f, 0.0f, -3.0f),
      glm::vec3(0.0f, 0.0f, 45.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["barrel"],
    {
      _texture_atlas["barrel.diffusion"],
      _texture_atlas["barrel.specular"],
    },
    {
      glm::vec3(0.0f, 0.0f, -3.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["wooden_box"],
    {
      _texture_atlas["wooden_box"],
      _texture_atlas["random_specular"],
    },
    {
      glm::vec3(-3.0f, 0.0f, -3.0f),
      glm::vec3(0.0f, 45.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
    }
  ));

  // smg_D
  // smg_G
  // smg_N
  // smg_S
  _objects.push_back(new object(
    *_mesh_atlas["smg"],
    {
      _texture_atlas["smg.diffusion"],
      _texture_atlas["smg.specular"],
    },
    {
      glm::vec3(0.0f, 5.0f, -3.0f),
      glm::vec3(0.0f, 90.0f, 0.0f),
      glm::vec3(0.4f, 0.4f, 0.4f),
    }
  ));

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // _default_shader->set_uniform_4f("uni_color", { 1.0f, 1.0f, 1.0f, 1.0f });

  // (can this be deleted??) glBindTexture(GL_TEXTURE_2D, 0);
  // _default_shader->unbind();
  _lighting_scene_shader->unbind();

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

    const float delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(passed_time).count();

    glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, _draw_wireframe ? GL_LINE : GL_FILL);

    _event_queue->poll();

    _camera->update(*_input, delta_time);

    // cache camera matrices
    glm::mat4 view = _camera->view();
    glm::mat4 projection = _camera->projection();

    // draw light source (always first object)
    _lighting_source_shader->bind();

    object* light_source = _objects[0];

    _lighting_source_shader->set_uniform_matrix_4fv("uni_view_matrix", view);
    _lighting_source_shader->set_uniform_matrix_4fv("uni_projection_matrix", projection);
    _lighting_source_shader->set_uniform_matrix_4fv("uni_model_matrix", light_source->model());

    light_source->draw(*_lighting_source_shader);

    _lighting_source_shader->unbind();

    // draw rest of the objects
    _lighting_scene_shader->bind();

    _lighting_scene_shader->set_uniform_3f("uni_view_position", _camera->position());

    object* floor = _objects[1];

    glm::mat4 model = floor->model();
    glm::mat3 normal = glm::transpose(glm::inverse(model));

    _lighting_scene_shader->set_uniform_matrix_4fv("uni_view_matrix", view);
    _lighting_scene_shader->set_uniform_matrix_4fv("uni_projection_matrix", projection);
    _lighting_scene_shader->set_uniform_matrix_4fv("uni_model_matrix", model);
    _lighting_scene_shader->set_uniform_matrix_3fv("uni_normal_matrix", normal);

    floor->draw(*_lighting_scene_shader);

    std::size_t object_count = _objects.size();
    for (std::size_t i = 2; i < object_count; ++i) {
      object* temp_object = _objects[i];

      if (i == 2) {
        temp_object->rotate(glm::vec3(1.0f, 0.0f, 0.0f), 50 * delta_time);
      } else if (i == 3) {
        temp_object->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 50 * delta_time);
      } else if (i == 4) {
        temp_object->rotate(glm::vec3(0.0f, 0.0f, 1.0f), 50 * delta_time);
      }

      model = temp_object->model();
      normal = glm::transpose(glm::inverse(model));

      _lighting_scene_shader->set_uniform_matrix_4fv("uni_model_matrix", model);
      _lighting_scene_shader->set_uniform_matrix_3fv("uni_normal_matrix", normal);
      
      temp_object->draw(*_lighting_scene_shader);
    }

    _lighting_scene_shader->unbind();

    glfwSwapBuffers(_context);

    frames++;
  }
}

void terminate() {
  delete _default_shader;
  delete _lighting_scene_shader;
  delete _lighting_source_shader;

  for (auto [name, mesh] : _mesh_atlas) {
    delete mesh;
  }

  _mesh_atlas.clear();

  for (auto [name, texture] : _texture_atlas) {
    delete texture;
  }

  _texture_atlas.clear();

  for (object* object : _objects) {
    delete object;
  }

  _objects.clear();

  delete _input;
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

// Not actually async at the time
template<typename T, typename... Args>
T* _load_async(Args&&... args) {
  return new T(std::forward<Args>(args)...);
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
