#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>

#include <application/window_module.hpp>

#include <rendering/rendering_module.hpp>
#include <rendering/shader.hpp>

#include <physics/physics_module.hpp>
#include <physics/rigidbody.hpp>

class demo_module final : public sbx::module {

  class demo_system final : public sbx::system {

  public:

    demo_system(sbx::event_queue* event_queue, sbx::scene* scene)
    : _event_queue{event_queue},
      _scene{scene},
      _resource_cache{resource_cache} { }

    ~demo_system() = default;

    void initialize() override {
      
    }

    void update(sbx::time delta_time) override {
      static_cast<void>(delta_time);
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
    const auto player = create_entity();
    emplace_component<sbx::rigidbody>(player, glm::vec3{0, 0, 0}, 5.0f);

    load_resource<sbx::shader>(
      "default_shader", 
      "resources/shaders/default/vertex.glsl", 
      "resources/shaders/default/fragment.glsl"
    );

    add_system<demo_system>(_event_queue, _scene);
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
