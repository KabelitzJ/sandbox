#ifndef DEMO_DEBUG_SUBRENDERER_HPP_
#define DEMO_DEBUG_SUBRENDERER_HPP_

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/buffers/push_handler.hpp>
#include <libsbx/graphics/buffers/storage_handler.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

namespace demo {

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

  struct line {
    sbx::math::vector4 position;
    sbx::math::color color;
  }; // struct line

  debug_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : sbx::graphics::subrenderer{stage},
    _pipeline{path, stage} { }

  ~debug_subrenderer() override {

  }

  static auto add_line(const sbx::math::vector3& start, const sbx::math::vector3& end, const sbx::math::color& color) -> void {
    _lines.push_back(line{
      .position = sbx::math::vector4{start, 1.0f},
      .color = color
    });

    _lines.push_back(line{
      .position = sbx::math::vector4{end, 1.0f},
      .color = color
    });
  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    if (_lines.empty()) {
      return;
    }

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto camera_node = scene.camera();

    auto& camera = scene.get_component<sbx::scenes::camera>(camera_node);

    const auto& projection = camera.projection();

    const auto& camera_global_transform = scene.get_component<sbx::scenes::global_transform>(camera_node);

    const auto view = sbx::math::matrix4x4::inverted(camera_global_transform.model);
    
    _pipeline.bind(command_buffer);

    _storage_handler.push(std::span<const line>{_lines});

    _push_handler.push("mvp", projection * view);

    _descriptor_handler.push("buffer_vertex_data", _storage_handler);
    _descriptor_handler.push("push", _push_handler);

    // if (!_descriptor_handler.update(_pipeline)) {
    //   return;
    // }

    _descriptor_handler.update_set(0u);
    _descriptor_handler.bind_descriptors(command_buffer, 0u);
    _push_handler.bind(command_buffer, _pipeline);

    command_buffer.draw(_lines.size(), 1, 0, 0);

    _lines.clear();
  }

private:

  pipeline _pipeline;

  sbx::graphics::push_handler _push_handler;
  sbx::graphics::storage_handler _storage_handler;

  sbx::graphics::descriptor_handler _descriptor_handler;

  inline static auto _lines = std::vector<line>{};

}; // class debug_subrenderer

} // namespace demo

#endif // DEMO_DEBUG_SUBRENDERER_HPP_
