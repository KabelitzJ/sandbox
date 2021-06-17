#include "core.hpp"

namespace sbx {

template<typename T, typename... Args>
T* _load_async(Args&&... args) {
  return new T(std::forward<Args>(args)...);
}

void engine::start() {
  _initialize();
  _run();
  _terminate();
}

void engine::_initialize() {
  if (!glfwInit()) {
    std::cout << "[Error] Could not initialize glfw!\n";

    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();

  if (!primary_monitor) {
    std::cout << "[Error] Glfw could not detect a monitor!\n";

    return;
  }

  const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);

  const int width = video_mode->width;
  const int height = video_mode->height;

  _context = glfwCreateWindow(width, height, "Sandbox", primary_monitor, nullptr);

  if (!_context) {
    std::cout << "[Error] Glfw could not create a window!\n";

    return;
  }

  glfwMakeContextCurrent(_context);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "[Error] Could not load gl bindings (glad)!\n";

    return;
  }

  glfwSetInputMode(_context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(0);
  glfwFocusWindow(_context);

  _event_queue = new event_queue(_context);
  _event_queue->subscribe<key_pressed_event>([this](key_pressed_event& event){
    if (event.code == key_code::ESCAPE) {
      glfwSetWindowShouldClose(_context, true);
    }
  });
  _event_queue->subscribe<key_pressed_event>([this](key_pressed_event& event){
    if (event.code == key_code::ENTER) {
      _draw_wireframe = !_draw_wireframe;
    }
  });

  _input = new input_manager(*_event_queue);

  _scene = new scene();

  float aspect = static_cast<float>(width) / static_cast<float>(height);

  _camera = new perspective_camera(_camera_position, _camera_direction, _camera_speed, _fov, aspect, 0.1f, 1000.0f, _camera_pitch, _camera_yaw);

  glViewport(0, 0, width, height);

  _gui_projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

  _default_shader = new shader("resources/shaders/default_vertex.glsl", "resources/shaders/default_fragment.glsl");
  _lighting_source_shader = new shader("resources/shaders/lighting_source_vertex.glsl", "resources/shaders/lighting_source_fragment.glsl");
  _lighting_scene_shader = new shader("resources/shaders/lighting_scene_vertex.glsl", "resources/shaders/lighting_scene_fragment.glsl");
  _text_shader = new shader("resources/shaders/text_vertex.glsl", "resources/shaders/text_fragment.glsl");

  _lighting_scene_shader->bind();

  // Setting up static shader uniforms
  _lighting_scene_shader->set_uniform_1f("uni_material.shininess", 32.0f);
  _lighting_scene_shader->set_uniform_3f("uni_light.position", { 0.0f, 0.0f, 0.0f });
  _lighting_scene_shader->set_uniform_3f("uni_light.ambient", { 0.2f, 0.2f, 0.2f });
  _lighting_scene_shader->set_uniform_3f("uni_light.diffuse", { 0.5f, 0.5f, 0.5f });
  _lighting_scene_shader->set_uniform_3f("uni_light.specular", { 1.0f, 1.0f, 1.0f });

  // loading meshes
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

  std::srand(std::time(nullptr));

  const int size = 200;
  const int vertex_count = 200;

  std::vector<mesh::vertex> vertices;
  std::vector<GLuint> indices;

  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;

  for (int y = 0; y < vertex_count; ++y) {
    for (int x = 0; x < vertex_count; ++x) {
      const float position_x = -static_cast<float>(y) / (static_cast<float>(vertex_count) - 1.0f) * size;
      const float position_y = 0.0f;
      const float position_z = -static_cast<float>(x) / (static_cast<float>(vertex_count) - 1.0f) * size;
      const glm::vec3 position(position_x, position_y, position_z);

      const float uv_x = static_cast<float>(y) / (static_cast<float>(vertex_count) - 1.0f);
      const float uv_y = static_cast<float>(x) / (static_cast<float>(vertex_count) - 1.0f);
      const glm::vec2 uv(uv_x, uv_y);

      const glm::vec3 normal(0.0f, 1.0f, 0.0f);

      vertices.push_back({ position, uv, normal });
    }
  }

  for (int y = 0; y < vertex_count - 1; ++y) {
    for (int x = 0; x < vertex_count - 1; ++x) {
      const int top_left = y * vertex_count + x;
      const int top_right = top_left + 1;
      const int bottom_left = (y + 1) * vertex_count + x;
      const int bottom_right = bottom_left + 1;

      indices.push_back(top_left);
      indices.push_back(top_right);
      indices.push_back(bottom_left);
      indices.push_back(top_right);
      indices.push_back(bottom_right);
      indices.push_back(bottom_left);
    }
  }

  _mesh_atlas.emplace("custom", _load_async<mesh>(vertices, indices));

  // loading textures
  _texture_atlas.emplace("blank", _load_async<texture>("resources/textures/blank.jpg"));
  _texture_atlas.emplace("filled", _load_async<texture>("resources/textures/filled.jpg"));
  _texture_atlas.emplace("brick_wall", _load_async<texture>("resources/textures/brick_wall.jpg"));
  _texture_atlas.emplace("cobble_wall", _load_async<texture>("resources/textures/cobble_wall.jpg"));
  _texture_atlas.emplace("lava_diffusion", _load_async<texture>("resources/textures/lava_diffusion.jpg"));
  _texture_atlas.emplace("lava_specular", _load_async<texture>("resources/textures/lava_specular.jpg"));
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
    *_mesh_atlas["custom"],
    {
      _texture_atlas["lava_diffusion"],
      _texture_atlas["lava_specular"],
    },
    {
      glm::vec3(100.0f, -4.0f, 100.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.5f, 1.0f),
    }
  ));

  _objects.push_back(new object(
    *_mesh_atlas["torus"],
    {
      _texture_atlas["lava_diffusion"],
      _texture_atlas["lava_specular"],
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
      _texture_atlas["filled"],
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
    *_mesh_atlas["cube"],
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
  
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  _lighting_scene_shader->unbind();
}

#include <ft2build.h>
#include FT_FREETYPE_H

struct character {
  unsigned int id;
  glm::vec2 size; 
  glm::vec2 bearing;
  unsigned int advance;
};

static std::unordered_map<char, character> _characters;

void engine::render_text(GLuint VAO, GLuint VBO, const std::string& text, float x, float y, glm::vec3 color, float scale) {
  _text_shader->bind();
  _text_shader->set_uniform_3f("color", color);
  _text_shader->set_uniform_matrix_4fv("projection", _gui_projection);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  for (char c : text) {
    character character = _characters[c];

    float xpos = x + character.bearing.x * scale;
    float ypos = y - (character.size.y - character.bearing.y) * scale;

    float w = character.size.x * scale;
    float h = character.size.y * scale;

    float vertives[6][4] = {
      { xpos,     ypos + h, 0.0f, 0.0f },
      { xpos,     ypos,     0.0f, 1.0f },
      { xpos + w, ypos,     1.0f, 1.0f },

      { xpos,     ypos + h, 0.0f, 0.0f },
      { xpos + w, ypos,     1.0f, 1.0f },
      { xpos + w, ypos + h, 1.0f, 0.0f }
    };

    glBindTexture(GL_TEXTURE_2D, character.id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertives), vertives);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    x += (character.advance >> 6) * scale;
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  _text_shader->unbind();
}

void engine::_run() {
  std::chrono::nanoseconds frame_time(0);
  std::uint32_t frames = 0;
  std::uint32_t last_frames = 0;
  std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

  FT_Library ft;

  if (FT_Init_FreeType(&ft)) {
    std::cout << "Could not initialize FreeType!\n";

    return;
  }

  // resources/fonts/UbuntuMono-R.ttf
  // resources/fonts/FreeMono.ttf

  FT_Face face;
  if (FT_New_Face(ft, "resources/fonts/FreeMono.ttf", 0, &face)) {
    std::cout << "Could not load font!\n";

    return;
  }

  FT_Set_Pixel_Sizes(face, 0, 48);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "Could not load glyph '" << c << "'!\n";

      continue;
    }

    unsigned char* buffer = face->glyph->bitmap.buffer;
    unsigned int width = face->glyph->bitmap.width;
    unsigned int rows = face->glyph->bitmap.rows;
    unsigned int left = face->glyph->bitmap_left;
    unsigned int top = face->glyph->bitmap_top;
    unsigned int advance = face->glyph->advance.x;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, rows, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    character character = { id, glm::vec2(width, rows), glm::vec2(left, top), advance };

    _characters.insert(std::make_pair(c, character));
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(_context)) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds passed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_time);
    last_time = now;

    frame_time += passed_time;

    if (frame_time >= std::chrono::seconds(1)) {
      frame_time = std::chrono::nanoseconds(0);
      last_frames = frames;
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

    // Draw ui layer

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    auto cam_pos = _camera->position();

    auto x_pos_text = "x: " + std::to_string(cam_pos.x);
    auto y_pos_text = "y: " + std::to_string(cam_pos.y);
    auto z_pos_text = "z: " + std::to_string(cam_pos.z);

    const auto fps_text = std::string("fps:       ") + std::to_string(last_frames);
    const auto delta_time_text = std::string("delta:     ") + std::to_string(delta_time);
    const auto wireframe_text = std::string("wireframe: ") + (_draw_wireframe ? std::string("on") : std::string("off"));

    render_text(VAO, VBO, fps_text, 80.0f, 1000.0f, { 1.0f, 0.0f, 1.0f }, 0.5f);
    render_text(VAO, VBO, delta_time_text, 80.0f, 970.0f, { 1.0f, 0.0f, 1.0f }, 0.5f);
    render_text(VAO, VBO, wireframe_text, 80.0f, 940.0f, { 1.0f, 0.0f, 1.0f }, 0.5f);

    render_text(VAO, VBO, x_pos_text, 80.0f, 880.0f, { 1.0f, 0.0f, 0.0f }, 0.5f);
    render_text(VAO, VBO, y_pos_text, 80.0f, 850.0f, { 0.0f, 1.0f, 0.0f }, 0.5f);
    render_text(VAO, VBO, z_pos_text, 80.0f, 820.0f, { 0.0f, 0.0f, 1.0f }, 0.5f);

    render_text(VAO, VBO, "Caitlín is the best <3", 25.0f, 25.0f, { 0.8f, 0.4f, 0.3f });

    glfwSwapBuffers(_context);

    frames++;
  }

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void engine::_terminate() {
  delete _default_shader;
  delete _lighting_scene_shader;
  delete _lighting_source_shader;
  delete _text_shader;

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
  delete _scene;
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

} // namespace sbx
