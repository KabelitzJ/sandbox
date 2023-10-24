#ifndef LIBSBX_SHADOWS_PIPELINE_HPP_
#define LIBSBX_SHADOWS_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/shadows/vertex3d.hpp>

namespace sbx::shadows {

class pipeline : public graphics::graphics_pipeline<vertex3d> {

public:

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::graphics_pipeline<vertex3d>{path, stage, graphics::pipeline_definition{ .uses_depth = true, .uses_transparency = false }} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::shadows

#endif // LIBSBX_SHADOWS_PIPELINE_HPP_
