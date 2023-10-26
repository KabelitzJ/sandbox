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
    _pipeline{path, stage} { }

  ~ui_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& ui_module = core::engine::get_module<ui::ui_module>();
    auto& devices_module = core::engine::get_module<devices::devices_module>();

    auto& window = devices_module.window();

    _uniform_handler.push("projection", math::matrix4x4::orthographic(0.0f, static_cast<float>(window.width()), static_cast<float>(window.height()), 0.0f));

    const auto& widgets = ui_module.widgets();

    for (const auto& id : _used_uniforms) {
      _uniform_data.erase(id);
    }

    _used_uniforms.clear();

    for (const auto& widget : widgets) {
      _used_uniforms.insert(widget->id());
      _render_widget(*widget, command_buffer);
    }
  }

private:

  auto _render_widget(widget& widget, graphics::command_buffer& command_buffer) -> void {
    const auto& id = widget.id();

    _pipeline.bind(command_buffer);

    auto& uniform_data = _uniform_data[id];

    auto& uniform_handler = uniform_data.uniform_handler;
    auto& descriptor_handler = uniform_data.descriptor_handler;
    auto& mesh = uniform_data.mesh;

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

  std::unordered_map<math::uuid, uniform_data> _uniform_data;
  std::unordered_set<math::uuid> _used_uniforms;

  graphics::uniform_handler _uniform_handler;

}; // class ui_subrenderer

} // namespace sbx::ui 

#endif // LIBSBX_UI_UI_SUBRENDERER_HPP_
