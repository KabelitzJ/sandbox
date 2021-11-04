#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>
#include <application/window_module.hpp>
#include <rendering/rendering_module.hpp>

struct velocity {
  float x{};
  float y{};
  float z{};
};

struct position {
  float x{};
  float y{};
  float z{};

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

class demo_module final : public sbx::module {

  class demo_system : public sbx::system {

  public:

    demo_system(sbx::event_queue* event_queue, sbx::scene* scene)
    : _event_queue{event_queue},
      _scene{scene} { }

    ~demo_system() = default;

    void initialize() override {

    }

    void update(sbx::time delta_time) override {
      auto view = _scene->create_view<position, const velocity>();

      for (auto entity : view) {
        auto [position, velocity] = view.get(entity);

        position += velocity * delta_time;
      }
    }

    void terminate() override {
      
    }

  private:

    sbx::event_queue* _event_queue{};
    sbx::scene* _scene{};

  };

public:

  demo_module() = default;
  ~demo_module() = default;

  void initialize() override {
    const auto player = _scene->create_entity();
    _scene->emplace_component<position>(player, 0.0f, 0.0f);
    _scene->emplace_component<velocity>(player, 0.5f, 0.5f);
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<sbx::window_module>();
  engine.add_module<sbx::rendering_module>();
}
