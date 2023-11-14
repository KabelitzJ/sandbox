#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iterator>
#include <utility>

#include <libsbx/units/units.hpp>
#include <libsbx/utility/utility.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/bitmaps/bitmaps.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>
#include <libsbx/audio/audio.hpp>
#include <libsbx/ui/ui.hpp>
#include <libsbx/shadows/shadows.hpp>
#include <libsbx/physics/physics.hpp>

class demo_renderer : public sbx::graphics::renderer {

public:

  demo_renderer() {
    {
      auto attachments = std::vector<sbx::graphics::attachment>{
        sbx::graphics::attachment{0, "shadow", sbx::graphics::attachment::type::image, VK_FORMAT_R8_UNORM}
      };

      auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
        sbx::graphics::subpass_binding{0, {0}}
      };

      add_render_stage(std::move(attachments), std::move(subpass_bindings), sbx::graphics::viewport{sbx::math::vector2u{4096, 4096}});
    }

    {
      auto attachments = std::vector<sbx::graphics::attachment>{
        sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain},
        sbx::graphics::attachment{1, "depth", sbx::graphics::attachment::type::depth}
      };

      auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
        sbx::graphics::subpass_binding{0, {0, 1}},
        sbx::graphics::subpass_binding{1, {0}}
      };

      add_render_stage(std::move(attachments), std::move(subpass_bindings));
    }
  }

  ~demo_renderer() override = default;

  auto initialize() -> void override {
    add_subrenderer<sbx::shadows::shadow_subrenderer>("res://shaders/shadow", sbx::graphics::pipeline::stage{0, 0});

    add_subrenderer<sbx::models::mesh_subrenderer>("res://shaders/cell_shading", sbx::graphics::pipeline::stage{1, 0});
    // add_subrenderer<sbx::ui::ui_subrenderer>("res://shaders/ui", sbx::graphics::pipeline::stage{1, 1});
  }

}; // class demo_renderer

class demo_application : public sbx::core::application {

public:

  demo_application()
  : sbx::core::application{} {
    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();
    
    assets_module.set_asset_directory("./demo/assets");

    auto base_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/base.png");
    auto default_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/default.png");
    auto grid_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/grid.png");
    auto prototype_black_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/prototype_black.png");
    auto white_id = assets_module.load_asset<sbx::graphics::image2d>("res://textures/white.png");

    // _texture_id = base_id;

    auto monkey_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/suzanne.obj");
    auto sphere_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/sphere.obj");
    auto cube_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/cube.obj");
    auto tree_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/tree.obj");
    auto plane_id = assets_module.load_asset<sbx::models::mesh>("res://meshes/plane.obj");

    // _mesh_id = sphere_id;

    auto font_jet_brains_mono_id = assets_module.load_asset<sbx::ui::font>("res://fonts/JetBrainsMono-Medium.ttf", sbx::ui::pixels{18u});
    auto font_roboto_id = assets_module.load_asset<sbx::ui::font>("res://fonts/Roboto-Regular.ttf", sbx::ui::pixels{18u});

    auto ambience_birds_sound_id = assets_module.load_asset<sbx::audio::sound_buffer>("res://audio/ambience.wav");
    auto forest_sound_id = assets_module.load_asset<sbx::audio::sound_buffer>("res://audio/forest.wav");

    auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

    auto& container = ui_module.container();

    _label_fps = container.add_widget<sbx::ui::label>("FPS:  0", sbx::math::vector2u{25, 25}, font_jet_brains_mono_id, sbx::math::color{0.53f, 0.01f, 0.01f, 1.0f});
    _label_delta_time = container.add_widget<sbx::ui::label>("Delta time: 0 ms", sbx::math::vector2u{25, 50}, font_jet_brains_mono_id, sbx::math::color{0.53f, 0.01f, 0.01f, 1.0f});

    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    graphics_module.set_renderer<demo_renderer>();

    auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

    auto& window = devices_module.window();

    window.set_icon("res://icons/sandbox.png");

    window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
      sbx::core::engine::quit();
    };

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.load_scene("res://scenes/demo.yaml");

    // [Todo] KAJ 2023-08-16 15:30 - This should probably be done automatically
    scene.start();

    window.show();
  }

  ~demo_application() override {

  }

  auto update() -> void  {
    if (sbx::devices::input::is_key_pressed(sbx::devices::key::escape)) {
      sbx::core::engine::quit();
    }

    // auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
    // auto& scene = scenes_module.scene();

    // if (_flag && !_cube) {
    //   _cube = scene.create_node("Cube", sbx::math::transform{sbx::math::vector3{-5.0f, 10.0f, 0.0f}, sbx::math::vector3::zero, sbx::math::vector3::one});

    //   _cube->add_component<sbx::scenes::static_mesh>(_mesh_id, _texture_id);

    //   // auto& script = _cube->add_component<sbx::scenes::script>("res://scripts/rotate.lua");
    //   // script.set("speed", -120.0f);

    //   auto& rigidbody = _cube->add_component<sbx::physics::rigidbody>(1.0f, 0.75f, false);
    //   rigidbody.set_acceleration(sbx::math::vector3{0.0f, -9.81f, 0.0f});

    //   _cube->add_component<sbx::physics::box_collider>(sbx::math::vector3{1.0f, 1.0f, 1.0f});
    // } else if (!_flag && _cube) {
    //   scene.destroy_node(*_cube);
    //   _cube.reset();
    // }

    // auto camera_node = scene.camera();
    // auto& camera = camera_node.get_component<sbx::scenes::camera>();

    // if (sbx::devices::input::is_key_pressed(sbx::devices::key::r)) {
    //   camera.set_field_of_view(sbx::math::degree{75.0f});
    // } else {
    //   const auto& scroll = sbx::devices::input::scroll_delta();

    //   auto field_of_view = camera.field_of_view().to_degrees() - scroll.y * 10.0f;

    //   if (field_of_view > 75.0f) {
    //     field_of_view = sbx::math::degree{75.0f};
    //   } else if (field_of_view < 30.0f) {
    //     field_of_view = sbx::math::degree{30.0f};
    //   }

    //   camera.set_field_of_view(field_of_view);
    // }

    const auto delta_time = sbx::core::engine::delta_time();

    _time += sbx::units::second{delta_time};

    if (_time >= sbx::units::second{1.0f}) {
      _label_fps->set_text(fmt::format("FPS: {}", _frames));
      _time -= sbx::units::second{1.0f};
      _frames = 0;
    } else {
      ++_frames;
    }

    _label_delta_time->set_text(fmt::format("Delta time: {:.2f} ms", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value()));
  }

private:

  sbx::memory::observer_ptr<sbx::ui::label> _label_fps;
  sbx::memory::observer_ptr<sbx::ui::label> _label_delta_time;

  // std::optional<sbx::scenes::node> _cube;
  // sbx::assets::asset_id _mesh_id;
  // sbx::assets::asset_id _texture_id;
  // bool _flag = false;

  sbx::units::second _time;
  std::uint32_t _frames;

}; // class demo_application

auto sbx::core::create_application() -> std::unique_ptr<sbx::core::application> {
  return std::make_unique<demo_application>();
}
