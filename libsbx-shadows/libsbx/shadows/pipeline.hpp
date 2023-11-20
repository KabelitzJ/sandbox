#ifndef LIBSBX_SHADOWS_PIPELINE_HPP_
#define LIBSBX_SHADOWS_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/shadows/vertex3d.hpp>

namespace sbx::shadows {

class pipeline : public graphics::graphics_pipeline<vertex3d> {

  inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::front,
      .front_face = graphics::front_face::counter_clockwise,
      .depth_bias = graphics::depth_bias{
        .constant_factor = 1.25f,
        .clamp = 0.0f,
        .slope_factor = 1.75f
      }
    }
  };

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::graphics_pipeline<vertex3d>{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_PIPELINE_HPP_
