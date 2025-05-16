#ifndef LIBSBX_SCENES_DEBUG_SUBRENDERER_HPP_
#define LIBSBX_SCENES_DEBUG_SUBRENDERER_HPP_

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/scenes/scenes_module.hpp>

namespace sbx::scenes {

class debug_subrenderer final : public sbx::graphics::subrenderer {

  class pipeline : public sbx::graphics::graphics_pipeline<sbx::graphics::empty_vertex> {

    inline static constexpr auto pipeline_definition = sbx::graphics::pipeline_definition{
      .depth = sbx::graphics::depth::read_only,
      .uses_transparency = true,
      .rasterization_state = sbx::graphics::rasterization_state{
        .polygon_mode = sbx::graphics::polygon_mode::fill,
        .cull_mode = sbx::graphics::cull_mode::none,
        .front_face = sbx::graphics::front_face::counter_clockwise
      },
      .primitive_topology = sbx::graphics::primitive_topology::line_list
    };
  
    using base_type = sbx::graphics::graphics_pipeline<sbx::graphics::empty_vertex>;
  
  public:
  
    using vertex_type = sbx::graphics::empty_vertex;
  
    pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
    : base_type{path, stage, pipeline_definition} { }
  
    ~pipeline() override = default;
  
  }; // class pipeline

public:

  debug_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage},
    _pipeline{path, stage},
    _storage_handler{VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT},
    _descriptor_handler{_pipeline, 0u} { }

  ~debug_subrenderer() override {

  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    const auto& lines = scenes_module.debug_lines();

    if (lines.empty()) {
      return;
    }

    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = scene.get_component<sbx::scenes::camera>(camera_node);

    const auto& projection = camera.projection();

    const auto& camera_global_transform = scene.get_component<sbx::scenes::global_transform>(camera_node);

    const auto view = sbx::math::matrix4x4::inverted(camera_global_transform.model);
    
    _pipeline.bind(command_buffer);

    _storage_handler.push(std::span<const scenes_module::line>{lines.data(), lines.size()});

    _push_handler.push("mvp", projection * view);

    _descriptor_handler.push("buffer_vertex_data", _storage_handler);
    _descriptor_handler.push("push", _push_handler);

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);
    _push_handler.bind(command_buffer, _pipeline);

    command_buffer.draw(lines.size(), 1, 0, 0);

    scenes_module.debug_lines();
  }

private:

  pipeline _pipeline;

  sbx::graphics::push_handler _push_handler;
  sbx::graphics::storage_handler _storage_handler;

  sbx::graphics::descriptor_handler _descriptor_handler;

}; // class debug_subrenderer

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_DEBUG_SUBRENDERER_HPP_
