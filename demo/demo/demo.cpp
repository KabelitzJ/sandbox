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
#include <libsbx/async/async.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/scenes/scenes.hpp>
#include <libsbx/ui/ui.hpp>

class demo_renderer : public sbx::graphics::renderer {

public:

  demo_renderer() { 
    auto render_pass_attachments_1 = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, VK_FORMAT_UNDEFINED},
      sbx::graphics::attachment{1, "depth", sbx::graphics::attachment::type::depth}
    };

    auto render_pass_subpass_bindings_1 = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}},
      sbx::graphics::subpass_binding{1, {0}}
    };

    add_render_stage(std::move(render_pass_attachments_1), std::move(render_pass_subpass_bindings_1));
  }

  ~demo_renderer() override {

  }

  auto initialize() -> void override {
    add_subrenderer<sbx::scenes::scene_subrenderer>(sbx::graphics::pipeline::stage{0, 0}, "./demo/assets/shaders/basic");
    add_subrenderer<sbx::ui::ui_subrenderer>(sbx::graphics::pipeline::stage{0, 1}, "./demo/assets/shaders/ui");
  }

}; // class demo_renderer

class demo_application : public sbx::core::application {

public:

  demo_application() {
    auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

    auto& window = devices_module.window();

    window.on_window_closed_signal() += [this]([[maybe_unused]] const auto& event){
      quit();
    };

    window.on_key_pressed() += [this]([[maybe_unused]] const auto& event){
      if (event.key == GLFW_KEY_ESCAPE) {
        quit();
      }
    };

    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    graphics_module.set_renderer<demo_renderer>();

    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    auto base_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/base.png");
    auto default_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/default.png");
    auto grid_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/grid.png");

    auto monkey_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/suzanne.obj");
    auto sphere_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/sphere.obj");
    auto cube_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/cube.obj");

    auto font_jet_brains_mono_id = assets_module.load_asset<sbx::ui::font>("./demo/assets/fonts/JetBrainsMono-Medium.ttf", 16u);
    auto font_roboto_id = assets_module.load_asset<sbx::ui::font>("./demo/assets/fonts/Roboto-Regular.ttf", 16u);

    auto& ui_module = sbx::core::engine::get_module<sbx::ui::ui_module>();

    ui_module.add_widget<sbx::ui::label>("Hello, World!", sbx::math::vector2u{25, 25}, font_roboto_id, sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f});

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto sun = scene.create_node("Sun");
    sun.add_component<sbx::scenes::static_mesh>(sphere_id, base_id);
    auto& sun_rotation = sun.add_component<sbx::scenes::script>("./demo/assets/scripts/rotate.lua");
    sun_rotation.set("speed", 75.0f);

    auto earth = scene.create_child_node(sun, "Earth", sbx::math::transform{sbx::math::vector3{4.0f, 0.0f, 0.0f}, sbx::math::vector3::zero, sbx::math::vector3{0.5f, 0.5f, 0.5f}});
    earth.add_component<sbx::scenes::static_mesh>(sphere_id, base_id);
    auto& earth_rotation = earth.add_component<sbx::scenes::script>("./demo/assets/scripts/rotate.lua");
    earth_rotation.set("speed", 100.0f);

    auto moon = scene.create_child_node(earth, "Moon", sbx::math::transform{sbx::math::vector3{2.0f, 0.0f, 0.0f}, sbx::math::vector3::zero, sbx::math::vector3{0.3f, 0.3f, 0.3f}});
    moon.add_component<sbx::scenes::static_mesh>(sphere_id, base_id);
    auto& moon_rotation = moon.add_component<sbx::scenes::script>("./demo/assets/scripts/rotate.lua");
    moon_rotation.set("speed", 125.0f);

    auto camera = scene.create_camera(sbx::math::degree{90.0f}, window.aspect_ratio(), 0.1f, 1000.0f, "Camera");
    camera.add_component<sbx::scenes::script>("./demo/assets/scripts/camera.lua");

    // [Todo] KAJ 2023-08-16 15:30 - This should probably be done automatically
    scene.start();

    window.show();
  }

  ~demo_application() override {

  }

  auto update() -> void  {

  }

}; // class demo_application

auto sbx::core::create_application() -> std::unique_ptr<sbx::core::application> {
  return std::make_unique<demo_application>();
}
