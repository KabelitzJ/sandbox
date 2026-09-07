// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_TEXT_LAYOUT_HPP_
#define LIBSBX_CANVAS_TEXT_LAYOUT_HPP_

#include <cstdint>
#include <vector>

#include <libsbx/math/vector4.hpp>

#include <libsbx/canvas/components.hpp>
#include <libsbx/canvas/rect_resolve.hpp>

namespace sbx::canvas {

struct text_glyph {
  resolved_rect rect;
  math::vector4 uv_rect;
  std::uint32_t texture_index;
}; // struct text_glyph

[[nodiscard]] auto shape_text(const ui_text& text, const resolved_rect& rect) -> std::vector<text_glyph>;

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_TEXT_LAYOUT_HPP_
