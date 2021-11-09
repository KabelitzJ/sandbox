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

    demo_system(sbx::event_queue* event_queue, sbx::scene* scene, sbx::resource_cache* resource_cache)
    : _event_queue{event_queue},
      _scene{scene},
      _resource_cache{resource_cache} { }

    ~demo_system() = default;

    void initialize() override {
      
    }

    void update(sbx::time delta_time) override {
      static_cast<void>(delta_time);
      auto shader = _resource_cache->get<sbx::shader>("default_shader");

      shader->bind();

      shader->unbind();
    }

    void terminate() override {
      
    }

  private:

    sbx::event_queue* _event_queue{};
    sbx::scene* _scene{};
    sbx::resource_cache* _resource_cache{};

  };

public:

  demo_module() = default;
  ~demo_module() = default;

  void initialize() override {
    const auto player = _scene->create_entity();
    _scene->emplace_component<sbx::rigidbody>(player, glm::vec3{0, 0, 0}, 5.0f);

    _resource_cache->load<sbx::shader>(
      "default_shader", 
      "resources/shaders/default/vertex.glsl", 
      "resources/shaders/default/fragment.glsl"
    );

    _scheduler->add_system<demo_system>(_event_queue, _scene, _resource_cache);
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
