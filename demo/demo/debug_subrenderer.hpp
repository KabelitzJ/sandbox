#ifndef DEMO_DEBUG_SUBRENDERER_HPP_
#define DEMO_DEBUG_SUBRENDERER_HPP_

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

namespace demo {

class debug_subrenderer final : public sbx::graphics::subrenderer {

  class pipeline : public sbx::graphics::graphics_pipeline<sbx::graphics::empty_vertex> {

    inline static constexpr auto pipeline_definition = sbx::graphics::pipeline_definition{
      .depth = sbx::graphics::depth::read_write,
      .uses_transparency = false,
      .rasterization_state = sbx::graphics::rasterization_state{
        .polygon_mode = sbx::graphics::polygon_mode::fill,
        .cull_mode = sbx::graphics::cull_mode::none,
        .front_face = sbx::graphics::front_face::counter_clockwise
      }
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

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    
  }

private:

  pipeline _pipeline;

}; // class debug_subrenderer

} // namespace demo

#endif // DEMO_DEBUG_SUBRENDERER_HPP_
