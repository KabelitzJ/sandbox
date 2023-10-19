#ifndef LIBSBX_MODELS_PIPELINE_HPP_
#define LIBSBX_MODELS_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

class pipeline : public graphics::graphics_pipeline<vertex3d> {

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::graphics_pipeline<vertex3d>{path, stage, graphics::pipeline_definition{ .uses_depth = true, .uses_transparency = false }} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::models

#endif // LIBSBX_MODELS_PIPELINE_HPP_
