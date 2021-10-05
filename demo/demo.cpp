#include <iostream>

#include <core/core.hpp>
#include <ecs/process.hpp>

struct position {
  float x;
  float y;
  float z;
};

struct velocity {
  float x;
  float y;
  float z;
};

#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class my_process final : public sbx::process<my_process> {

public:
  my_process() = default;
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
  }

  void update(const sbx::fast_time delta) {
    glfwPollEvents();

    if (glfwGetKey(_handle, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(_handle, true);
    }

    if (glfwWindowShouldClose(_handle)) {
      succeed();
    }

    auto title = std::stringstream{};

    title << "Test [Delta: " << delta << "]";

    glfwSetWindowTitle(_handle, title.str().c_str());

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

};

class my_system final : public sbx::system {

public:
  my_system() {

  }

  ~my_system() {

  }

  void initialize() override {
    _scheduler->attach<my_process>();

    const auto player = _registry->create_entity();

    (void) player;
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
