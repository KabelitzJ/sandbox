#ifndef DEMO_PIPELINE_HPP_
#define DEMO_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <demo/terrain/vertex.hpp>

namespace demo {

class pipeline : public sbx::graphics::graphics_pipeline<demo::vertex> {

  inline static constexpr auto pipeline_definition = sbx::graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = false,
    .rasterization_state = sbx::graphics::rasterization_state{
      .polygon_mode = sbx::graphics::polygon_mode::fill,
      .cull_mode = sbx::graphics::cull_mode::back,
      .front_face = sbx::graphics::front_face::counter_clockwise
    }
  };

  using base = sbx::graphics::graphics_pipeline<demo::vertex>;

public:

  using vertex_type = demo::vertex;

  pipeline(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace demo

#endif // DEMO_PIPELINE_HPP_
