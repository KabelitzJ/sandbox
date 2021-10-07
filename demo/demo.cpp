#include <iostream>

#include <core/core.hpp>
#include <ecs/process.hpp>
#include <ecs/entity.hpp>

struct velocity {
  float x;
  float y;
  float z;
};

struct position {
  float x;
  float y;
  float z;

  position& operator+=(const velocity& velocity) {
    x += velocity.x;
    y += velocity.y;
    z += velocity.z;

    return *this;
  }
};

position operator+(position position, const velocity& velocity) {
  position += velocity;

  return position;
}

std::ostream& operator<<(std::ostream& os, const position& position) {
  return os << "[" << position.x << ", " << position.y << ", " << position.z << "]";
}

velocity operator*(const velocity& velocity, const float scalar) {
  return {velocity.x * scalar, velocity.y * scalar, velocity.z * scalar};
}

#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class my_process final : public sbx::process<my_process> {

public:
  my_process(sbx::registry& registry)
  : _handle{nullptr},
    _registry{&registry},
    _player{sbx::null} { }
    
  ~my_process() = default;

  void initialize() {
    std::cout << "init\n";

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _handle = glfwCreateWindow(960, 720, "Test [Delta: 0]", nullptr, nullptr);

    glfwMakeContextCurrent(_handle);

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    glfwSwapInterval(0);

    _player = _registry->create_entity();

    _registry->emplace_component<position>(_player, 0.0f, 0.0f, 0.0f);
    _registry->emplace_component<velocity>(_player, -0.4f, 1.0f, 0.0f);
  }

  void update(const sbx::fast_time delta_time) {
    glfwPollEvents();

    if (glfwGetKey(_handle, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(_handle, true);
    }

    if (glfwWindowShouldClose(_handle)) {
      succeed();
    }

    auto title = std::stringstream{};

    title << "Test [Delta: " << delta_time << "]";

    glfwSetWindowTitle(_handle, title.str().c_str());

    auto view = _registry->view<position, velocity>();

    for (auto entity : view) {
      [[maybe_unused]] auto& pos = view.get<position>(entity);
      [[maybe_unused]] auto& vel = view.get<velocity>(entity);
    }

    glfwSwapBuffers(_handle);
  }

  void succeeded() {
    _exit();
  }

  void failed() {
    _exit();
  }

private:
  void _exit() {
    glfwDestroyWindow(_handle);
    glfwTerminate();

    std::cout << "exit\n";
  }

  GLFWwindow* _handle{nullptr};
  sbx::registry* _registry{nullptr};
  sbx::entity _player{sbx::null};

};

class my_system final : public sbx::system {

public:
  my_system() {

  }

  ~my_system() {

  }

  void initialize() override {
    _scheduler->attach<my_process>(*_registry);
  }

};

class my_module final : public sbx::module {

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    add_system<my_system>();

    const auto e = _registry->create_entity();
    
    _registry->emplace_component<velocity>(e, 0.0f, 0.0f, 0.0f);
    _registry->emplace_component<position>(e, 0.0f, 0.0f, 0.0f);
  }

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
