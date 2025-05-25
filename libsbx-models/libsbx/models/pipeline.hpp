#ifndef LIBSBX_MODELS_PIPELINE_HPP_
#define LIBSBX_MODELS_PIPELINE_HPP_

#include <fstream>

#include <nlohmann/json.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

template<bool UsesTransparency, graphics::cull_mode CullMode>
class pipeline : public graphics::graphics_pipeline {

  inline static const auto pipeline_definition = graphics::pipeline_definition{
    .depth = UsesTransparency ? graphics::depth::read_only : graphics::depth::read_write,
    .uses_transparency = UsesTransparency,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = CullMode,
      .front_face = graphics::front_face::counter_clockwise
    },
    // .vertex_input = graphics::vertex_input<models::vertex3d>::description()
  };

  using base_type = graphics::graphics_pipeline;

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage, const VkSpecializationInfo* specialization_info)
  : base_type{path, stage, pipeline_definition, specialization_info} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::models

#endif // LIBSBX_MODELS_PIPELINE_HPP_
