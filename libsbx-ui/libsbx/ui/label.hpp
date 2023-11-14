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
    _color{color},
    _font_id{font_id} { }

  ~label() override = default;

  auto set_text(const std::string& text) -> void {
    _is_dirty = text != _text;
    _text = text;
  }

  auto update(graphics::uniform_handler& uniform, graphics::descriptor_handler& descriptor_handler) -> void override {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    auto& atlas = font.atlas();

    uniform.push("color", _color);

    descriptor_handler.push("atlas", atlas.image());
  }

  auto render(graphics::command_buffer& command_buffer, std::unique_ptr<mesh>& mesh) -> void override {
    if (_is_dirty) {
      _update_mesh(mesh);

      _is_dirty = false;
    }

    if (!mesh) {
      throw std::runtime_error{"Failed to create mesh for label"};
    }

    mesh->render(command_buffer);
  }

private:

  auto _update_mesh(std::unique_ptr<mesh>& mesh) -> void {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    auto& font = assets_module.get_asset<ui::font>(_font_id);

    auto vertices = std::vector<ui::vertex2d>{};
    auto indices = std::vector<std::uint32_t>{};

    auto position_x = static_cast<std::float_t>(_position.x);
    auto position_y = static_cast<std::float_t>(_position.y);

    for (const auto& character : _text) {
      auto& glyph = font.glyph(character);

      auto x = position_x + static_cast<std::float_t>(glyph.bearing.x);
      auto y = position_y - static_cast<std::float_t>(glyph.size.y - glyph.bearing.y);

      auto w = static_cast<std::float_t>(glyph.size.x);
      auto h = static_cast<std::float_t>(glyph.size.y);

      {
        auto position = math::vector2{x, y + h};
        auto uv = math::vector2{glyph.uv_position.x, glyph.uv_position.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      {
        auto position = math::vector2{x, y};
        auto uv = math::vector2{glyph.uv_position.x, glyph.uv_position.y + glyph.uv_size.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      {
        auto position = math::vector2{x + w, y};
        auto uv = math::vector2{glyph.uv_position.x + glyph.uv_size.x, glyph.uv_position.y + glyph.uv_size.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      {
        auto position = math::vector2{x, y + h};
        auto uv = math::vector2{glyph.uv_position.x, glyph.uv_position.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      {
        auto position = math::vector2{x + w, y};
        auto uv = math::vector2{glyph.uv_position.x + glyph.uv_size.x, glyph.uv_position.y + glyph.uv_size.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      {
        auto position = math::vector2{x + w, y + h};
        auto uv = math::vector2{glyph.uv_position.x + glyph.uv_size.x, glyph.uv_position.y};

        vertices.emplace_back(position, uv);
        indices.emplace_back(vertices.size() - 1);
      }

      position_x += static_cast<std::float_t>(glyph.advance);
    }

    mesh = std::make_unique<ui::mesh>(std::move(vertices), std::move(indices));
  }

  std::string _text;
  bool _is_dirty;
  math::color _color;
  assets::asset_id _font_id;

}; // class label

} // namespace sbx::ui

#endif // LIBSBX_UI_LABEL_HPP_
