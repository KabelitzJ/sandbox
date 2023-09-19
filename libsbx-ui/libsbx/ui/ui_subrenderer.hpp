#ifndef LIBSBX_UI_UI_SUBRENDERER_HPP_
#define LIBSBX_UI_UI_SUBRENDERER_HPP_

#include <filesystem>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/models/buffer.hpp>
#include <libsbx/models/mesh.hpp>

#include <libsbx/ui/vertex2d.hpp>
#include <libsbx/ui/ui_module.hpp>

namespace sbx::ui {

class ui_subrenderer : public graphics::subrenderer {

public:

  ui_subrenderer(const graphics::pipeline::stage& stage, const std::filesystem::path& path)
  : graphics::subrenderer{stage},
    _pipeline{stage, path, graphics::vertex_input<ui::vertex2d>::description()} {
    auto& ui_module = core::engine::get_module<ui::ui_module>();

    ui_module.on_widget_added() += [this](ui::widget& widget){
      const auto& id = widget.id();

      _uniform_data.insert({id, std::make_unique<uniform_data>()});
    };

    ui_module.on_widget_removed() += [this](ui::widget& widget){
      const auto& id = widget.id();

      _uniform_data.erase(id);
    };
  }

  ~ui_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    const auto& ui_module = core::engine::get_module<ui::ui_module>();

    const auto& widgets = ui_module.widgets();

    for (const auto& widget : widgets) {
      _render_widget(*widget, command_buffer);
    }
  }

private:

  auto _render_widget(widget& widget, graphics::command_buffer& command_buffer) -> void {
    const auto& id = widget.id();

    auto& [uniform_handler, descriptor_handler] = *_uniform_data.at(id);

    const auto& position = widget.position();
    const auto& size = widget.size();

    
  }

  struct uniform_data {
    graphics::uniform_handler uniform_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data

  graphics::graphics_pipeline _pipeline;

  std::unordered_map<math::uuid, std::unique_ptr<uniform_data>> _uniform_data;

}; // class ui_subrenderer

} // namespace sbx::ui 

#endif // LIBSBX_UI_UI_SUBRENDERER_HPP_
