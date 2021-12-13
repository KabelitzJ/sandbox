#include <iostream>

#include <core/entry_point.hpp>
#include <core/events.hpp>
#include <core/logger.hpp>
#include <core/camera.hpp>

#include <window/window_module.hpp>

#include <rendering/rendering_module.hpp>
#include <rendering/shader.hpp>
#include <rendering/mesh.hpp>
#include <rendering/model.hpp>
#include <rendering/texture.hpp>

#include <physics/physics_module.hpp>
#include <physics/rigidbody.hpp>

#include <types/vector.hpp>
#include <types/primitives.hpp>
#include <types/transform.hpp>

class demo_module final : public sbx::module {

  class demo_system final : public sbx::system {

  public:

    demo_system() : _angle{0.0f}, _time{0.0f} { }
    ~demo_system() = default;

    void initialize() override {

      // Loading shaders

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

      // Loading meshes

      load_resource<sbx::mesh>(
        "cube", 
        "resources/models/cube.obj"
      );

      load_resource<sbx::mesh>(
        "quad", 
        "resources/models/quad.obj"
      );

      load_resource<sbx::mesh>(
        "sphere",
        "resources/models/sphere.obj"
      );

      load_resource<sbx::mesh>(
        "monkey",
        "resources/models/monkey.obj"
      );

      load_resource<sbx::texture>(
        "default",
        "resources/textures/default.png"
      );

      load_resource<sbx::texture>(
        "red",
        "resources/textures/red.png"
      );

      load_resource<sbx::texture>(
        "prototype",
        "resources/textures/prototype.png"
      );

      load_resource<sbx::texture>(
        "placeholder",
        "resources/textures/placeholder/albedo.png"
      );

      // Creating entities

      const auto cube = create_entity();
      auto& cube_transform_component = get_components<sbx::transform>(cube);
      cube_transform_component.position = sbx::vector3{0.0f, 0.0f, 0.0f};
      add_component<sbx::rigidbody>(cube, sbx::vector3{0.0f, 0.8f, 0.0f}, 10.0f, true);
      add_component<sbx::model>(cube, "cube", "test_shader", "prototype");

      // Creating camera

      const auto camera = create_entity();
      auto& camera_transform_component = get_components<sbx::transform>(camera);
      camera_transform_component.position = sbx::vector3{0.0f, 0.0f, 3.0f};
      add_component<sbx::camera>(
        camera, 
        true,
        sbx::inverse(sbx::model_matrix_from_transform(camera_transform_component)),
        sbx::perspective(sbx::to_radians(90.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f)
      );
    }

    void update(const sbx::time delta_time) override {
      _angle += 1.5f * delta_time;
      _time += delta_time;

      if (_angle >= 360.0f) {
        _angle = 0.0f;
      }
      
      auto view = create_view<sbx::transform, sbx::model>();

      for (const auto entity : view) {
        auto [transform, model] = view.get(entity);

        transform.position.y = std::sin(_time) * 0.75f;
        transform.rotation = sbx::rotate(transform.rotation, sbx::vector3_up * _angle);

      }
    }

    void terminate() override {
      
    }

  private:

    sbx::float32 _angle{};
    sbx::time _time{};

  };

public:

  demo_module() = default;
  ~demo_module() = default;

  void initialize() override {
    dispatch_event<sbx::clear_color_changed_event>(sbx::color{0.2f, 0.3f, 0.3f, 1.0f});

    add_listener<sbx::key_pressed_event>([this](const auto& event) {
      if (event.keycode == sbx::key::escape) {
        dispatch_event<sbx::window_closed_event>("Escape pressed");
      }
    });

    dispatch_event<sbx::set_mouse_visibility_event>(false);

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
