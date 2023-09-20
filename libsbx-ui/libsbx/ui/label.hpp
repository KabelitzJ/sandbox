#ifndef LIBSBX_UI_LABEL_HPP_
#define LIBSBX_UI_LABEL_HPP_

#include <string>
#include <memory>

#include <libsbx/core/logger.hpp>

#include <libsbx/assets/asset.hpp>
#include <libsbx/assets/assets_module.hpp>

#include <libsbx/graphics/images/image2d.hpp>

#include <libsbx/ui/widget.hpp>
#include <libsbx/ui/font.hpp>

namespace sbx::ui {

class label : public widget {

public:

  label(const std::string& text, const math::vector2u& position, const math::vector2u& size, assets::asset_id font_id)
  : widget{position, size},
    _text{text},
    _font_id{font_id} { }

  ~label() override = default;

  auto render(graphics::command_buffer& command_buffer) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    for (const auto& character : _text) {
      const auto& glyph = font.glyph(character);
      const auto& atlas = font.atlas();
    }
  }

private:

  std::string _text;
  assets::asset_id _font_id;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
