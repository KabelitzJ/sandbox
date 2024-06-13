#ifndef LIBSBX_GIZMOS_PIPELINE_HPP_
#define LIBSBX_GIZMOS_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::gizmos {

class pipeline : public graphics::graphics_pipeline<models::vertex3d> {

  inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::back,
      .front_face = graphics::front_face::counter_clockwise
    }
  };

  using base = graphics::graphics_pipeline<models::vertex3d>;

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : base{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::gizmos

#endif // LIBSBX_GIZMOS_PIPELINE_HPP_
