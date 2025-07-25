#ifndef LIBSBX_SHADOWS_PIPELINE_HPP_
#define LIBSBX_SHADOWS_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

// #include <libsbx/shadows/vertex3d.hpp>
#include <libsbx/models/vertex3d.hpp>

namespace sbx::shadows {

class pipeline : public graphics::graphics_pipeline {

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = graphics::depth::read_write,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::front,
      .front_face = graphics::front_face::counter_clockwise
    },
    .vertex_input = graphics::vertex_input<models::vertex3d>::description()
  };

  using base = graphics::graphics_pipeline;

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : base{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_PIPELINE_HPP_
