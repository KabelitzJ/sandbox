#ifndef LIBSBX_UI_UI_SUBRENDERER_HPP_
#define LIBSBX_UI_UI_SUBRENDERER_HPP_

#include <filesystem>

#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/ui/vertex2d.hpp>
#include <libsbx/ui/mesh.hpp>
#include <libsbx/ui/pipeline.hpp>
#include <libsbx/ui/ui_module.hpp>

namespace sbx::ui {

class ui_subrenderer : public graphics::subrenderer {

public:

  ui_subrenderer(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : graphics::subrenderer{stage},
    _pipeline{path, stage} {
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
    auto& ui_module = core::engine::get_module<ui::ui_module>();
    auto& devices_module = core::engine::get_module<devices::devices_module>();

    auto& window = devices_module.window();

    _uniform_handler.push("projection", math::matrix4x4::orthographic(0.0f, static_cast<float>(window.width()), static_cast<float>(window.height()), 0.0f));

    const auto& widgets = ui_module.widgets();

    for (const auto& widget : widgets) {
      _render_widget(*widget, command_buffer);
    }
  }

private:

  auto _render_widget(widget& widget, graphics::command_buffer& command_buffer) -> void {
    const auto& id = widget.id();

    _pipeline.bind(command_buffer);

    auto& [uniform_handler, descriptor_handler, mesh] = *_uniform_data.at(id);

    widget.update(uniform_handler, descriptor_handler);

    descriptor_handler.push("uniform_scene", _uniform_handler);
    descriptor_handler.push("uniform_object", uniform_handler);

    if (!descriptor_handler.update(_pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    widget.render(command_buffer, mesh);
  }

  struct uniform_data {
    graphics::uniform_handler uniform_handler;
    graphics::descriptor_handler descriptor_handler;
    std::unique_ptr<mesh> mesh;
  }; // struct uniform_data

  pipeline _pipeline;

  std::unordered_map<math::uuid, std::unique_ptr<uniform_data>> _uniform_data;

  graphics::uniform_handler _uniform_handler;

}; // class ui_subrenderer

} // namespace sbx::ui 

#endif // LIBSBX_UI_UI_SUBRENDERER_HPP_