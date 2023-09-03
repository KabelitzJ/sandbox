#ifndef LIBSBX_UI_UI_SUBRENDERER_HPP_
#define LIBSBX_UI_UI_SUBRENDERER_HPP_

#include <filesystem>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/ui/vertex.hpp>

namespace sbx::ui {

class ui_subrenderer : public graphics::subrenderer {

public:

  ui_subrenderer(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::subrenderer{stage},
    _pipeline{stage, path, graphics::vertex_input<ui::vertex>::description()} { }

  ~ui_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {

  }

private:

  graphics::graphics_pipeline _pipeline;

}; // class ui_subrenderer

} // namespace sbx::ui 

#endif // LIBSBX_UI_UI_SUBRENDERER_HPP_
