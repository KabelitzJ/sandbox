// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_CANVAS_DRAW_LIST_HPP_
#define LIBSBX_CANVAS_CANVAS_DRAW_LIST_HPP_

#include <cstdint>
#include <vector>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

namespace sbx::canvas {

struct canvas_vertex {
  math::vector4 position;
  math::color color;
  math::vector2 uv;
  std::uint32_t texture_index;
  std::uint32_t padding;
}; // struct canvas_vertex

static_assert(sizeof(canvas_vertex) == 48u, "canvas_vertex must stay byte-mirrored with shaders/passes/canvas.slang's canvas_vertex struct");

class canvas_draw_list final {

public:

  auto add_quad(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size) -> void;

  auto add_glyph_quad(const math::vector2& top_left, const math::vector2& size, const math::vector4& uv_rect, std::uint32_t texture_index, const math::color& color, const math::vector2& screen_size) -> void;

  [[nodiscard]] auto vertices() const noexcept -> const std::vector<canvas_vertex>& {
    return _vertices;
  }

  [[nodiscard]] auto glyph_vertices() const noexcept -> const std::vector<canvas_vertex>& {
    return _glyph_vertices;
  }

  auto clear() noexcept -> void {
    _vertices.clear();
    _glyph_vertices.clear();
  }

private:

  std::vector<canvas_vertex> _vertices{};
  std::vector<canvas_vertex> _glyph_vertices{};

}; // class canvas_draw_list

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_CANVAS_DRAW_LIST_HPP_
