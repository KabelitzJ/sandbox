#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <core/core.hpp>
#include <util/random.hpp>

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

struct on_window_closed { };

class my_module final : public sbx::module {

  class my_other_system : public sbx::system<my_other_system> {

    public:
      my_other_system(sbx::event_queue* event_queue)
      : _event_queue{event_queue},
        _handle{nullptr},
        _frame_counter{0},
        _timer{0.0f} { }

      ~my_other_system() = default;

      void initialize() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        auto* monitor = glfwGetPrimaryMonitor();

        const auto* video_mode = glfwGetVideoMode(monitor);

        const auto width = video_mode->width / 2;
        const auto height = video_mode->height / 2;
        const auto x = video_mode->width / 2 - width / 2;
        const auto y = video_mode->height / 2 - height / 2;

        _handle = glfwCreateWindow(width, height, "Window [fps: 0]", nullptr, nullptr);

        glfwSetWindowPos(_handle, x, y);

        glfwMakeContextCurrent(_handle);

        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        glClearColor(0.7f, 0.3f, 0.4f, 1.0f);

        glfwSwapInterval(0);

        glfwSetFramebufferSizeCallback(_handle, [](auto*, auto buffer_width, auto buffer_height){
          glViewport(0, 0, buffer_width, buffer_height);
        });
      }

      void update(const sbx::time delta_time) {
        glfwPollEvents();

        if (glfwGetKey(_handle, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
          glfwSetWindowShouldClose(_handle, true);
        }

        if (glfwWindowShouldClose(_handle)) {
          _event_queue->emplace_back<on_window_closed>();
          finish();
          return;
        }

        _timer += delta_time;

        if (_timer >= sbx::time{1.0f}) {
          auto ss = std::stringstream{};
          ss << "Window [fps: " << _frame_counter << "]";

          glfwSetWindowTitle(_handle, ss.str().c_str());

          _timer = 0.0f;
          _frame_counter = 0;
        }


        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(_handle);

        ++_frame_counter;
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
      sbx::uint32 _frame_counter{};
      sbx::time _timer{};
  };

  class my_system : public sbx::system<my_system> {

  public:
    my_system(sbx::event_queue* event_queue, sbx::registry* registry)
    : _registry{registry},
      _event_queue{event_queue},
      _entities{} { }

    ~my_system() = default;

    void initialize() {
      _event_queue->add_listener<on_window_closed>([this](const auto&){
        finish();
      });

      for (auto i = std::size_t{0u}; i < 100; ++i) {
        const auto pos = position{
          sbx::random::next<sbx::float32>(-10.0f, 10.0f),
          0.0f,
          sbx::random::next<sbx::float32>(-10.0f, 10.0f)
        };

        const auto vel = velocity{
          sbx::random::next<sbx::float32>(-1.0f, 1.0f),
          0.0f,
          sbx::random::next<sbx::float32>(-1.0f, 1.0f)
        };

        _entities.push_back(_registry->create_entity());
        _registry->emplace_component<position>(_entities[i], pos);
        _registry->emplace_component<velocity>(_entities[i], vel);
      }
    }

    void update(const sbx::time delta_time) {
      auto view = _registry->view<position, const velocity>();

      static_cast<void>(view);

      for (auto& entity : _entities) {
        auto& v = _registry->get_component<velocity>(entity);

        _registry->patch_component<position>(entity, [&v, &delta_time](auto& pp) { pp += v * delta_time; });
      }
    }

    void finished() {
      
    }

    void aborted() {
      
    }

  private:
    sbx::registry* _registry{};
    sbx::event_queue* _event_queue{};
    std::vector<sbx::entity> _entities{};

  }; // class my_system

public:
  my_module() {

  }

  ~my_module() {

  }

  void initialize() override {
    _scheduler->attach<my_system>(_event_queue, _registry);
    _scheduler->attach<my_other_system>(_event_queue);
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<my_module>();
}
