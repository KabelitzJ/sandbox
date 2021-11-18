#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>
#include <core/logger.hpp>
#include <types/transform.hpp>
#include <core/camera.hpp>

#include <application/window_module.hpp>

#include <rendering/rendering_module.hpp>
#include <rendering/shader.hpp>
#include <rendering/mesh.hpp>
#include <rendering/model.hpp>

#include <physics/physics_module.hpp>
#include <physics/rigidbody.hpp>

#include <types/vector.hpp>

class demo_module final : public sbx::module {

  class demo_system final : public sbx::system {

  public:

    demo_system() = default;
    ~demo_system() = default;

    void initialize() override {
      load_resource<sbx::shader>(
        "default_shader", 
        "resources/shaders/default/vertex.glsl", 
        "resources/shaders/default/fragment.glsl"
      );

      load_resource<sbx::shader>(
        "test_shader", 
        "resources/shaders/test/vertex.glsl", 
        "resources/shaders/test/fragment.glsl"
      );

      load_resource<sbx::mesh>(
        "cube", 
        "resources/models/cube.obj"
      );

      load_resource<sbx::mesh>(
        "quad", 
        "resources/models/quad.obj"
      );

      const auto player = create_entity();
      // emplace_component<sbx::rigidbody>(player, sbx::vector3{0, 0, 0}, 10.0f, false);
      emplace_component<sbx::model>(player, "cube", "test_shader");

      const auto camera = create_entity();
      emplace_component<sbx::camera>(
        camera, 
        true,
        sbx::look_at({5.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, sbx::vector3_up),
        sbx::perspective(sbx::to_radians(45.0f), 960.0f / 720.0f, 0.1f, 1000.0f)
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
