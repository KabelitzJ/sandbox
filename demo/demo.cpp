#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>
#include <core/logger.hpp>
#include <core/transform.hpp>

#include <application/window_module.hpp>

#include <rendering/rendering_module.hpp>
#include <rendering/shader.hpp>

#include <physics/physics_module.hpp>
#include <physics/rigidbody.hpp>

#include <types/vector.hpp>

class demo_module final : public sbx::module {

  class demo_system final : public sbx::system {

  public:

    demo_system() = default;
    ~demo_system() = default;

    void initialize() override {
      const auto player = create_entity();
      emplace_component<sbx::rigidbody>(player, sbx::vector3{0, 0, 0}, 10.0f, false);

      load_resource<sbx::shader>(
        "default_shader", 
        "resources/shaders/default/vertex.glsl", 
        "resources/shaders/default/fragment.glsl"
      );
    }

    void update(sbx::time delta_time) override {
      static_cast<void>(delta_time);
    }

    void terminate() override {
      
    }

  };

public:

  demo_module() = default;
  ~demo_module() = default;

  void initialize() override {
    add_system<demo_system>();
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
