#include <iostream>

#include <core/core.hpp>

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

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sstream>

struct on_window_closed { };

class my_module final : public sbx::module {

  class my_other_system : public sbx::system<my_other_system> {

    public:
      my_other_system(sbx::event_queue* event_queue)
      : _event_queue{event_queue},
        _handle{nullptr} { }

      ~my_other_system() = default;

      void initialize() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _handle = glfwCreateWindow(100, 100, "Window [delta: ]", nullptr, nullptr);

        glfwMakeContextCurrent(_handle);

        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        glClearColor(0.7f, 0.3f, 0.4f, 1.0f);

        glfwSwapInterval(0);
      }

      void update(const sbx::time delta_time) {
        glfwPollEvents();

        if (glfwGetKey(_handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
          glfwSetWindowShouldClose(_handle, true);
        }

        if (glfwWindowShouldClose(_handle)) {
          _event_queue->emplace<on_window_closed>();
          finish();
          return;
        }

        auto ss = std::stringstream{};
        ss << "Window [delta: " << delta_time << "]";

        glfwSetWindowTitle(_handle, ss.str().c_str());

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(_handle);
      }

      void finished() {
        glfwDestroyWindow(_handle);
        glfwTerminate();
      }

      void aborted() {
        
      }

    private:
      sbx::event_queue* _event_queue{};
      GLFWwindow* _handle;
  };

  class my_system : public sbx::system<my_system> {

  public:
    my_system(sbx::event_queue* event_queue)
    : _event_queue{event_queue} { }

    ~my_system() = default;

    void initialize() {
      _event_queue->add_listener<on_window_closed>([this](const auto&){
        finish();
      });
    }

    void update([[maybe_unused]]const sbx::time delta_time) {

    }

    void finished() {
      
    }

    void aborted() {
      
    }

  private:
    sbx::event_queue* _event_queue{};

  }; // class my_system

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    _scheduler->attach<my_system>(_event_queue);
    _scheduler->attach<my_other_system>(_event_queue);
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
