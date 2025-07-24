#ifndef LIBSBX_ANIMATIONS_PIPELINE_HPP_
#define LIBSBX_ANIMATIONS_PIPELINE_HPP_

#include <fstream>

#include <nlohmann/json.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/animations/vertex3d.hpp>

namespace sbx::animations {

class pipeline : public graphics::graphics_pipeline {

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = graphics::depth::read_write,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::none,
      .front_face = graphics::front_face::counter_clockwise
    },
    // .vertex_input = graphics::vertex_input<models::vertex3d>::description()
  };

  using base_type = graphics::graphics_pipeline;

public:

  pipeline(const std::filesystem::path& path, const graphics::render_graph::pass& pass)
  : base_type{path, pass, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_PIPELINE_HPP_
