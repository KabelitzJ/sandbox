#ifndef LIBSBX_UI_UI_SUBRENDERER_HPP_
#define LIBSBX_UI_UI_SUBRENDERER_HPP_

#include <filesystem>

#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/devices/devices_module.hpp>
#include <libsbx/devices/window.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffers/uniform_handler.hpp>

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
    auto vertices = std::vector<vertex2d>{
      vertex2d{math::vector2f{0.5f, -0.5f}, math::vector2f{1.0f, 0.0f}},
      vertex2d{math::vector2f{-0.5f, -0.5f}, math::vector2f{0.0f, 1.0f}},
      vertex2d{math::vector2f{0.5f, 0.5f}, math::vector2f{0.0f, 0.0f}},
      vertex2d{math::vector2f{-0.5f, 0.5f}, math::vector2f{1.0f, 1.0f}}
    };

    auto indices = std::vector<std::uint32_t>{
      1, 2, 0,
      1, 3, 2
    };

    _mesh = std::make_unique<mesh>(std::move(vertices), std::move(indices));
  }

  ~ui_subrenderer() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& ui_module = core::engine::get_module<ui::ui_module>();
    auto& devices_module = core::engine::get_module<devices::devices_module>();

    auto& window = devices_module.window();

    _uniform_handler.push("projection", math::matrix4x4::orthographic(0.0f, static_cast<float>(window.width()), static_cast<float>(window.height()), 0.0f));

    const auto& container = ui_module.container();
    const auto& widgets = container.widgets();

    for (auto entry = _uniform_data.begin(); entry != _uniform_data.end();) {
      if (_used_uniforms.contains(entry->first)) {
        ++entry;
      } else {
        entry = _uniform_data.erase(entry);
      }
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

    widget.update(uniform_handler, descriptor_handler);

    descriptor_handler.push("uniform_scene", _uniform_handler);
    descriptor_handler.push("uniform_object", uniform_handler);

    if (!descriptor_handler.update(_pipeline)) {
      return;
    }

    descriptor_handler.bind_descriptors(command_buffer);

    widget.render(command_buffer, _mesh);
  }

  struct uniform_data {
    graphics::uniform_handler uniform_handler;
    graphics::descriptor_handler descriptor_handler;
  }; // struct uniform_data

  pipeline _pipeline;

  std::unique_ptr<mesh> _mesh;

  std::unordered_map<math::uuid, uniform_data> _uniform_data;
  std::unordered_set<math::uuid> _used_uniforms;

  graphics::uniform_handler _uniform_handler;

}; // class ui_subrenderer

} // namespace sbx::ui 

#endif // LIBSBX_UI_UI_SUBRENDERER_HPP_
