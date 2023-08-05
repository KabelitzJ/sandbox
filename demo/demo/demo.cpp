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

class demo_subrenderer : public sbx::graphics::subrenderer {

public:

  demo_subrenderer(const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage},
    _pipeline{std::make_unique<sbx::graphics::graphics_pipeline>(stage, "./demo/assets/shaders/basic", sbx::graphics::vertex_input<sbx::models::vertex3d>::description())},
    _uniforms{_pipeline->find_descriptor_block("buffer_object")} {
    auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

    auto& window = devices_module.window();

    _camera_position = sbx::math::vector3{2.0f, 2.0f, 1.0f};

    _light_position = sbx::math::vector3{-1.0f, 3.0f, 1.0f};

    _uniform_buffer_object.model = sbx::math::matrix4x4::identity;
    _uniform_buffer_object.view = sbx::math::matrix4x4::look_at(_camera_position, sbx::math::vector3{0.0f, 0.0f, 0.0f}, sbx::math::vector3::up);
    _uniform_buffer_object.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
    _uniform_buffer_object.normal = sbx::math::matrix4x4::identity;
  }

  ~demo_subrenderer() override = default;

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& devices_module = sbx::core::engine::get_module<sbx::devices::devices_module>();

    auto& window = devices_module.window();

    auto& assets_module = sbx::core::engine::get_module<sbx::assets::assets_module>();

    auto& mesh = assets_module.get_asset<sbx::models::mesh>("./demo/assets/meshes/suzanne.obj");
    auto& image = assets_module.get_asset<sbx::graphics::image2d>("./demo/assets/textures/base.png");

    const auto delta_time = sbx::core::engine::delta_time();

    _pipeline->bind(command_buffer);

    _uniform_buffer_object.model = sbx::math::matrix4x4::rotated(_uniform_buffer_object.model, sbx::math::vector3::up, sbx::math::degree{45.0f} * delta_time);
    _uniform_buffer_object.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
    _uniform_buffer_object.normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(_uniform_buffer_object.model));

    _uniforms.push("normal", _uniform_buffer_object.normal);
    _uniforms.push("view", _uniform_buffer_object.view);
    _uniforms.push("model", _uniform_buffer_object.model);
    _uniforms.push("projection", _uniform_buffer_object.projection);

    _pipeline->push(_uniforms);
    _pipeline->push("image", image);

    _pipeline->bind_descriptors(command_buffer);

    mesh.render(command_buffer);
  }

private:

  struct uniform_buffer_object {
    sbx::math::matrix4x4 model;
    sbx::math::matrix4x4 view;
    sbx::math::matrix4x4 projection;
    sbx::math::matrix4x4 normal;
  }; // struct uniform_buffer_object

  sbx::math::vector3 _camera_position;
  sbx::math::vector3 _light_position;

  std::unique_ptr<sbx::graphics::graphics_pipeline> _pipeline;

  sbx::graphics::uniform_handler _uniforms;
  uniform_buffer_object _uniform_buffer_object;

}; // class demo_subrenderer

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
    add_subrenderer<demo_subrenderer>(sbx::graphics::pipeline::stage{ .renderpass = 0, .subpass = 0 });
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

    assets_module.load_asset<sbx::graphics::image2d>("./demo/assets/textures/base.png");

    assets_module.load_asset<sbx::models::mesh>("./demo/assets/meshes/suzanne.obj");

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
