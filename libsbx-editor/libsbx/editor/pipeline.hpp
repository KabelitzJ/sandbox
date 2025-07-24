#ifndef LIBSBX_EDITOR_PIPELINE_HPP_
#define LIBSBX_EDITOR_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::editor {

class pipeline : public sbx::graphics::graphics_pipeline {

  inline static const auto pipeline_definition = sbx::graphics::pipeline_definition{
    .depth = graphics::depth::disabled,
    .uses_transparency = false,
    .rasterization_state = sbx::graphics::rasterization_state{
      .polygon_mode = sbx::graphics::polygon_mode::fill,
      .cull_mode = sbx::graphics::cull_mode::back,
      .front_face = sbx::graphics::front_face::counter_clockwise
    },
    .vertex_input = sbx::graphics::vertex_input<sbx::models::vertex3d>::description()
  };

  using base_type = sbx::graphics::graphics_pipeline;

public:

  pipeline(const std::filesystem::path& path, const sbx::graphics::render_graph::pass& pass)
  : base_type{path, pass, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::editor

#endif // LIBSBX_EDITOR_PIPELINE_HPP_
