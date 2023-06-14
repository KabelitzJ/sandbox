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
  : sbx::graphics::subrenderer{stage} {
    auto& window = sbx::devices::devices_module::get().window();

    _camera_position = sbx::math::vector3{2.0f, 2.0f, 1.0f};

    _light_position = sbx::math::vector3{-1.0f, 3.0f, 1.0f};

    _pipeline = std::make_unique<sbx::graphics::graphics_pipeline>(stage, "./demo/assets/shaders/basic");

    _model = std::make_unique<sbx::graphics::model>("./demo/assets/meshes/sphere.obj");

    _push_constant.ambient_color = _model->material().ambient();
    _push_constant.diffuse_color = _model->material().diffuse();
    _push_constant.specular_color = _model->material().specular();
    _push_constant.shininess = sbx::math::vector4{_model->material().shininess(), 0.0f, 0.0f, 0.0f};
    _push_constant.camera_position = sbx::math::vector4{_camera_position, 1.0f};
    _push_constant.light_color = sbx::math::color{1.0f, 1.0f, 1.0f, 1.0f};
    _push_constant.light_position = sbx::math::vector4{_light_position, 1.0f};

    _uniform.model = sbx::math::matrix4x4::identity;
    _uniform.inverse_model = sbx::math::matrix4x4::identity;
    _uniform.view = sbx::math::matrix4x4::look_at(_camera_position, sbx::math::vector3{0.0f, 0.0f, 0.0f}, sbx::math::vector3::up);
    _uniform.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);
  }

  ~demo_subrenderer() override = default;

  auto render(sbx::graphics::command_buffer& command_buffer, std::float_t delta_time) -> void override {
    auto& window = sbx::devices::devices_module::get().window();

    _uniform.model = sbx::math::matrix4x4::rotated(_uniform.model, sbx::math::vector3{0.0f, 0.0f, 1.0f}, sbx::math::degree{45.0f} * delta_time);
    _uniform.inverse_model = sbx::math::matrix4x4::inverted(_uniform.model);
    _uniform.projection = sbx::math::matrix4x4::perspective(sbx::math::radian{45.0f}, window.aspect_ratio(), 0.1f, 10.0f);

    _pipeline->bind(command_buffer);

    _pipeline->update_uniform(_uniform);

    _pipeline->bind_descriptor_set(command_buffer);
    _pipeline->update_push_constant(command_buffer, VK_SHADER_STAGE_FRAGMENT_BIT, _push_constant);

    _model->render(command_buffer, delta_time);
  }

private:

  sbx::math::vector3 _camera_position{};
  sbx::math::vector3 _light_position{};

  std::unique_ptr<sbx::graphics::graphics_pipeline> _pipeline{};
  std::unique_ptr<sbx::graphics::model> _model{};
  sbx::graphics::push_constant _push_constant{};
  sbx::graphics::uniform _uniform{};

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

    add_render_stage(std::make_unique<sbx::graphics::render_stage>(std::move(render_pass_attachments_1), std::move(render_pass_subpass_bindings_1)));

    add_subrenderer<demo_subrenderer>(sbx::graphics::pipeline::stage{0, 0});
    add_subrenderer<sbx::graphics::model_subrenderer>(sbx::graphics::pipeline::stage{0, 0});
  }

  ~demo_renderer() override {

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
