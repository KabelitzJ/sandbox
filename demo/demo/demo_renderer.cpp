#include <demo/demo_renderer.hpp>

#include <libsbx/core/engine.hpp>
#include <libsbx/models/models.hpp>

namespace demo {

class demo_pipeline : public sbx::graphics::graphics_pipeline<sbx::graphics::empty_vertex> {

  inline static constexpr auto pipeline_definition = sbx::graphics::pipeline_definition {
    .uses_depth = false,
    .uses_transparency = false,
    .rasterization_state = sbx::graphics::rasterization_state {
      .polygon_mode = sbx::graphics::polygon_mode::fill,
      .cull_mode = sbx::graphics::cull_mode::none,
      .front_face = sbx::graphics::front_face::counter_clockwise
    }
  };

public:

  using vertex_type = sbx::graphics::empty_vertex;

  demo_pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::graphics_pipeline<vertex_type>{path, stage, pipeline_definition} { }

  ~demo_pipeline() override = default;

}; // class pipeline

class demo_subrenderer : public sbx::graphics::subrenderer {

public:

  demo_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage, const std::unordered_map<std::string, std::string>& attachment_names)
  : sbx::graphics::subrenderer{stage},
    _attachment_names{attachment_names},
    _pipeline{path, stage} { }

  ~demo_subrenderer() override = default;

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = camera_node.get_component<sbx::scenes::camera>();

    if (!camera.is_active()) {
      sbx::core::logger::warn("Scene does not have an active camera");
      return;
    }

    const auto& camera_transform = camera_node.get_component<sbx::math::transform>();

    _uniform_handler.push("camera_position", camera_transform.position());

    const auto& scene_light = scene.light();

    const auto light_direction = scene_light.direction();

    const auto position = light_direction * -30.0f;

    const auto view = sbx::math::matrix4x4::look_at(position, position + light_direction, sbx::math::vector3::up);
    const auto projection = sbx::math::matrix4x4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);

    _uniform_handler.push("light_space", sbx::math::matrix4x4{projection * view});

    const auto time = std::fmod(sbx::core::engine::time().value() * scene.wind_speed(), 1.0f);
    _uniform_handler.push("time", time);

    _uniform_handler.push("light_direction", light_direction);
    _uniform_handler.push("light_color", scene_light.color());

    _pipeline.bind(command_buffer);

    _descriptor_handler.push("uniform_scene", _uniform_handler);

    for (const auto& [name, attachment] : _attachment_names) {
      _descriptor_handler.push(name, graphics_module.attachment(attachment));
    }

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);

    command_buffer.draw(6, 1, 0, 0);
  }

private:

  std::unordered_map<std::string, std::string> _attachment_names;

  demo_pipeline _pipeline;

  sbx::graphics::uniform_handler _uniform_handler;
  sbx::graphics::descriptor_handler _descriptor_handler;

}; // class demo_subrenderer

demo_renderer::demo_renderer()
: _clear_color{0.0f, 0.0f, 0.0f, 1.0f} {
  // Render stage 0: Deferred scene
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "depth", sbx::graphics::attachment::type::depth},
      sbx::graphics::attachment{1, "position", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{2, "normal", sbx::graphics::attachment::type::image, sbx::graphics::format::r32g32b32a32_sfloat, _clear_color},
      sbx::graphics::attachment{3, "albedo", sbx::graphics::attachment::type::image, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0, 1, 2, 3}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }

  // Render stage 1: Deferred resolve
  {
    auto attachments = std::vector<sbx::graphics::attachment>{
      sbx::graphics::attachment{0, "swapchain", sbx::graphics::attachment::type::swapchain, sbx::graphics::format::r8g8b8a8_unorm, _clear_color}
    };

    auto subpass_bindings = std::vector<sbx::graphics::subpass_binding>{
      sbx::graphics::subpass_binding{0, {0}},
    };

    add_render_stage(std::move(attachments), std::move(subpass_bindings));
  }
}

auto demo_renderer::initialize() -> void {
  // Render stage 0
  add_subrenderer<sbx::models::mesh_subrenderer>("demo/assets/shaders/deferred", sbx::graphics::pipeline::stage{0, 0});

  auto attachment_names = std::unordered_map<std::string, std::string>{
    {"position_image", "position"},
    {"normal_image", "normal"},
    {"albedo_image", "albedo"}
  };

  // Render stage 1
  add_subrenderer<demo_subrenderer>("demo/assets/shaders/resolve", sbx::graphics::pipeline::stage{1, 0}, attachment_names);
}

} // namespace demo
