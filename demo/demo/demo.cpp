#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iterator>
#include <utility>

#include <libsbx/utility/utility.hpp>
#include <libsbx/units/units.hpp>
#include <libsbx/memory/memory.hpp>
#include <libsbx/math/math.hpp>
#include <libsbx/core/core.hpp>
#include <libsbx/signals/signals.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/scripting/scripting.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp>
#include <libsbx/assets/assets.hpp>
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>

class demo_renderer : public sbx::graphics::renderer {

public:

  demo_renderer() { 
    auto render_pass_attachments_1 = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, VK_FORMAT_UNDEFINED},
      sbx::graphics::attachment{1, "depth", sbx::graphics::attachment::type::depth}
    };

    auto render_pass_subpass_bindings_1 = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1}}
    };

    add_render_stage(std::move(render_pass_attachments_1), std::move(render_pass_subpass_bindings_1));
  }

  ~demo_renderer() override {

  }

  auto initialize() -> void override {
    add_subrenderer<sbx::scenes::scene_subrenderer>(sbx::graphics::pipeline::stage{ .renderpass = 0, .subpass = 0 });
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

    auto& scripting_module = sbx::core::engine::get_module<sbx::scripting::scripting_module>();

    for (const auto& entry : std::filesystem::directory_iterator("./demo/assets/scripts")) {
      if (entry.is_regular_file()) {
        scripting_module.load_script(entry.path());
      }
    }

    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    graphics_module.set_renderer<demo_renderer>();

    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    auto base_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/base.png");
    auto default_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/default.png");
    auto grid_id = assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/grid.png");

    auto monkey_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/suzanne.obj");
    auto sphere_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/sphere.obj");
    auto cube_id = assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/cube.obj");

    auto pipeline_id = assets_module.load_asset<sbx::graphics::graphics_pipeline>(
      "./demo/assets/shaders/basic",
      sbx::graphics::pipeline::stage{ .renderpass = 0, .subpass = 0 }, 
      sbx::graphics::vertex_input<sbx::models::vertex3d>::description()
    );

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto monkey = scene.create_node("Monkey", sbx::scenes::transform{sbx::math::vector3{-3.0f, -3.0f, -1.0f}});
    monkey.add_component<sbx::scenes::static_mesh>(monkey_id, base_id, pipeline_id);

    auto sphere = scene.create_child_node(monkey, "Sphere", sbx::scenes::transform{sbx::math::vector3{-4.0f, 0.0f, 0.0f}});
    sphere.add_component<sbx::scenes::static_mesh>(sphere_id, default_id, pipeline_id);

    auto cube = scene.create_child_node(monkey, "Cube", sbx::scenes::transform{sbx::math::vector3{4.0f, 0.0f, 0.0f}});
    cube.add_component<sbx::scenes::static_mesh>(cube_id, grid_id, pipeline_id);

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
