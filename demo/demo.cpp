#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>

#include <application/window_module.hpp>

#include <rendering/rendering_module.hpp>
#include <rendering/shader.hpp>

#include <physics/physics_module.hpp>

class demo_module final : public sbx::module {

  class demo_system final : public sbx::system {

  public:

    demo_system(sbx::event_queue* event_queue, sbx::scene* scene)
    : _event_queue{event_queue},
      _scene{scene},
      _default_shader{nullptr} { }

    ~demo_system() = default;

    void initialize() override {
      _default_shader = new sbx::shader("resources/shaders/default/vertex.glsl", "resources/shaders/default/fragment.glsl");
    }

    void update(sbx::time delta_time) override {

    }

    void terminate() override {
      delete _default_shader;
    }

  private:

    sbx::event_queue* _event_queue{};
    sbx::scene* _scene{};
    sbx::shader* _default_shader{};

  };

public:

  demo_module() = default;
  ~demo_module() = default;

  void initialize() override {
    const auto player = _scene->create_entity();

    _scheduler->add_system<demo_system>(_event_queue, _scene);
  }

  void terminate() override {

  }

private:

};

void sbx::setup(sbx::engine& engine) {
  engine.add_module<sbx::window_module>();
  engine.add_module<sbx::rendering_module>();
  engine.add_module<sbx::physics_module>();
  engine.add_module<demo_module>();
}
