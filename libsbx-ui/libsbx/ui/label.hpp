#ifndef LIBSBX_UI_LABEL_HPP_
#define LIBSBX_UI_LABEL_HPP_

#include <string>
#include <memory>

#include <libsbx/core/logger.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/math/color.hpp>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/assets_module.hpp>

#include <libsbx/graphics/graphics_module.hpp>
#include <libsbx/graphics/images/image2d.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>

#include <libsbx/ui/widget.hpp>
#include <libsbx/ui/font.hpp>
#include <libsbx/ui/mesh.hpp>

namespace sbx::ui {

class label : public widget {

public:

  label(const std::string& text, const math::vector2u& position, assets::asset_id font_id, const math::color& color = math::color{1.0f, 1.0f, 1.0f, 1.0f})
  : widget{position},
    _text{text},
    _is_dirty{true},
    _transforms{},
    _color{color},
    _font_id{font_id} { }

  ~label() override = default;

  auto set_text(const std::string& text) -> void {
    _is_dirty = text != _text;
    _text = text;
  }

  auto update(graphics::descriptor_handler& descriptor_handler, graphics::uniform_handler& uniform_handler, graphics::storage_handler& storage_handler) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    uniform_handler.push("color", _color);

    if (_is_dirty) {
      _recalculate_transforms();
    }

    storage_handler.push(std::span<const math::matrix4x4>{_transforms.data(), _transforms.size()});

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    auto& atlas = font.atlas();

    descriptor_handler.push("atlas", atlas.image());
    descriptor_handler.push("uniform_object", uniform_handler);
    descriptor_handler.push("buffer_transforms", storage_handler);
  }

  auto render(graphics::command_buffer& command_buffer, std::unique_ptr<mesh>& mesh) -> void override {
    if (mesh) {
      mesh->render(command_buffer, _transforms.size());
    }
  }

private:

  auto _recalculate_transforms() -> void {
    _transforms.clear();

    for (const auto character : _text) {
      _transforms.push_back(math::matrix4x4::identity);
    }

    _is_dirty = false;
  }

  std::string _text;
  bool _is_dirty;
  std::vector<math::matrix4x4> _transforms;
  math::color _color;
  assets::asset_id _font_id;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
