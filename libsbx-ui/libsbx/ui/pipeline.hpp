#ifndef LIBSBX_UI_PIPELINE_HPP_
#define LIBSBX_UI_PIPELINE_HPP_

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/ui/vertex2d.hpp>

namespace sbx::ui {

class pipeline : public graphics::graphics_pipeline<ui::vertex2d> {

public:

  pipeline(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::graphics_pipeline<ui::vertex2d>{stage, path, graphics::pipeline_definition{ .uses_depth = true, .uses_transparency = true }} { }

  ~pipeline() override = default;

}; // class pipeline

} // namespace sbx::ui

#endif // LIBSBX_UI_PIPELINE_HPP_
