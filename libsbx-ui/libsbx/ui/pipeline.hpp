#ifndef LIBSBX_UI_PIPELINE_HPP_
#define LIBSBX_UI_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/ui/vertex2d.hpp>

namespace sbx::ui {

class pipeline : public graphics::graphics_pipeline<ui::vertex2d> {

  inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
    .depth = graphics::depth::read_write,
    .uses_transparency = true,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::none,
      .front_face = graphics::front_face::counter_clockwise
    }
  };

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::graphics_pipeline<ui::vertex2d>{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::ui

#endif // LIBSBX_UI_PIPELINE_HPP_
