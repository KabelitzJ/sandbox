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

  label(const std::string& text, const math::vector2u& position, const math::vector2u& size, assets::asset_id font_id)
  : widget{position, size},
    _text{text},
    _color{math::color{1.0f, 1.0f, 1.0f, 1.0f}},
    _font_id{font_id},
    _position{position},
    _size{size} {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    const auto character = _text[0];

    const auto& glyph = font.glyph(character);
    const auto& atlas = font.atlas();

    auto vertices = std::vector<ui::vertex2d>{};

    {
      auto position_x = static_cast<float>(_position.x);
      auto position_y = static_cast<float>(_position.y);

      auto uv_x = glyph.uvs.position.x / static_cast<std::float_t>(atlas.width());
      auto uv_y = glyph.uvs.position.y / static_cast<std::float_t>(atlas.height());

      vertices.push_back(ui::vertex2d{math::vector2{position_x, position_y}, math::vector2{uv_x, uv_y}});
    }

    {
      auto position_x = static_cast<float>(_position.x + _size.x);
      auto position_y = static_cast<float>(_position.y);

      auto uv_x = glyph.uvs.position.x + glyph.uvs.size.x / static_cast<std::float_t>(atlas.width());
      auto uv_y = glyph.uvs.position.y / static_cast<std::float_t>(atlas.height());

      vertices.push_back(ui::vertex2d{math::vector2{position_x, position_y}, math::vector2{uv_x, uv_y}});
    }

    {
      auto position_x = static_cast<float>(_position.x + _size.x);
      auto position_y = static_cast<float>(_position.y + _size.y);

      auto uv_x = glyph.uvs.position.x + glyph.uvs.size.x / static_cast<std::float_t>(atlas.width());
      auto uv_y = glyph.uvs.position.y + glyph.uvs.size.y / static_cast<std::float_t>(atlas.height());

      vertices.push_back(ui::vertex2d{math::vector2{position_x, position_y}, math::vector2{uv_x, uv_y}});
    }

    {
      auto position_x = static_cast<float>(_position.x);
      auto position_y = static_cast<float>(_position.y + _size.y);

      auto uv_x = glyph.uvs.position.x / static_cast<std::float_t>(atlas.width());
      auto uv_y = glyph.uvs.position.y + glyph.uvs.size.y / static_cast<std::float_t>(atlas.height());

      vertices.push_back(ui::vertex2d{math::vector2{position_x, position_y}, math::vector2{uv_x, uv_y}});
    }

    auto indices = std::vector<std::uint32_t>{
      0, 1, 3,
      1, 2, 3
    };

    _mesh = std::make_unique<ui::mesh>(std::move(vertices), std::move(indices));
  }

  ~label() override = default;

  auto update(graphics::uniform_handler& uniform, graphics::descriptor_handler& descriptor_handler) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    auto& atlas = font.atlas();

    uniform.push("color", _color);

    descriptor_handler.push("atlas", atlas.image());
  }

  auto render(graphics::command_buffer& command_buffer) -> void override {
    _mesh->render(command_buffer);
  }

private:

  std::string _text;
  math::color _color;
  assets::asset_id _font_id;
  math::vector2u _position;
  math::vector2u _size;
  std::unique_ptr<ui::mesh> _mesh;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
