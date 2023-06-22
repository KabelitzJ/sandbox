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
#include <libsbx/signal/signal.hpp>
#include <libsbx/ecs/ecs.hpp>
#include <libsbx/async/async.hpp>
#include <libsbx/io/io.hpp>
#include <libsbx/scripting/scripting.hpp>
#include <libsbx/devices/devices.hpp>
#include <libsbx/graphics/graphics.hpp> 
#include <libsbx/models/models.hpp>
#include <libsbx/scenes/scenes.hpp>

class demo_subrenderer : public sbx::graphics::subrenderer {

public:

  demo_subrenderer(const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage},
    _pipeline{std::make_unique<sbx::graphics::graphics_pipeline>(stage, "./demo/assets/shaders/basic", sbx::graphics::vertex_input<sbx::graphics::vertex3d>::description())},
    _model{std::make_unique<sbx::graphics::model>("./demo/assets/meshes/suzanne.obj")} {
    auto& window = sbx::devices::devices_module::get().window();

    _camera_position = sbx::math::vector3{2.0f, 2.0f, 1.0f};

    _light_position = sbx::math::vector3{-1.0f, 3.0f, 1.0f};

    _uniform_buffer_object.model = sbx::math::matrix4x4::identity;
    _uniform_buffer_object.view = sbx::math::matrix4x4::look_at(_camera_position, sbx::math::vector3{0.0f, 0.0f, 0.0f}, sbx::math::vector3::up);
    _uniform_buffer_object.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
    _uniform_buffer_object.normal = sbx::math::matrix4x4::identity;
  }

  ~demo_subrenderer() override = default;

  auto render(sbx::graphics::command_buffer& command_buffer, std::float_t delta_time) -> void override {
    auto& window = sbx::devices::devices_module::get().window();

    _pipeline->bind(command_buffer);

    _uniform_buffer_object.model = sbx::math::matrix4x4::rotated(_uniform_buffer_object.model, sbx::math::vector3{0.0f, 0.0f, 1.0f}, sbx::math::degree{45.0f} * delta_time);
    _uniform_buffer_object.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
    _uniform_buffer_object.normal = sbx::math::matrix4x4::transposed(sbx::math::matrix4x4::inverted(_uniform_buffer_object.model));

    _uniforms.push("normal", _uniform_buffer_object.normal);
    _uniforms.push("view", _uniform_buffer_object.view);
    _uniforms.push("model", _uniform_buffer_object.model);
    _uniforms.push("projection", _uniform_buffer_object.projection);

    _pipeline->push("buffer_object", _uniforms);

    _pipeline->bind_descriptors(command_buffer);

    _model->render(command_buffer, delta_time);
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

  std::unique_ptr<sbx::graphics::model> _model;


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
    auto& window = sbx::devices::devices_module::get().window();

    window.set_on_window_closed([this]([[maybe_unused]] const sbx::devices::window_closed_event& event){
      quit();
    });

    window.set_on_key([this]([[maybe_unused]] const sbx::devices::key_event& event){
      if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS) {
        quit();
      }
    });

    auto& scripting_module = sbx::scripting::scripting_module::get();

    for (const auto& entry : std::filesystem::directory_iterator("./demo/assets/scripts")) {
      if (entry.is_regular_file()) {
        scripting_module.load_script(entry.path());
      }
    }

    auto& graphics_module = sbx::graphics::graphics_module::get();

    graphics_module.set_renderer<demo_renderer>();

    window.show();
  }

  ~demo_application() override = default;

  auto update() -> void  {

  }

private:



}; // class demo_application

auto main(int argc, const char** argv) -> int {
  try {
    auto args = std::vector<std::string>{argv, argv + argc};
    auto engine = std::make_unique<sbx::core::engine>(std::move(args));

    engine->run<demo_application>();
  } catch(const std::exception& exception) {
    sbx::core::logger::error("demo", "{}", exception.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
