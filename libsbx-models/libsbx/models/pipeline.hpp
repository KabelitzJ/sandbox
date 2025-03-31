#ifndef LIBSBX_MODELS_PIPELINE_HPP_
#define LIBSBX_MODELS_PIPELINE_HPP_

#include <fstream>

#include <nlohmann/json.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

class pipeline : public graphics::graphics_pipeline<vertex3d> {

  inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = true,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::back,
      .front_face = graphics::front_face::counter_clockwise
    }
  };

  using base_type = graphics::graphics_pipeline<vertex3d>;

public:

  using vertex_type = vertex3d;

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : base_type{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::models

#endif // LIBSBX_MODELS_PIPELINE_HPP_
