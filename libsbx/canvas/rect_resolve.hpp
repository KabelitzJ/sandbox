// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_RECT_RESOLVE_HPP_
#define LIBSBX_CANVAS_RECT_RESOLVE_HPP_

#include <libsbx/math/vector2.hpp>

#include <libsbx/canvas/components.hpp>

namespace sbx::canvas {

struct resolved_rect {
  math::vector2 position{};
  math::vector2 size{};

  auto operator==(const resolved_rect&) const -> bool = default;
}; // struct resolved_rect

[[nodiscard]] inline auto resolve_rect(const rect_transform& rect, const resolved_rect& parent) -> resolved_rect {
  const auto offset_min = rect.anchored_position - math::vector2{rect.pivot.x() * rect.size_delta.x(), rect.pivot.y() * rect.size_delta.y()};
  const auto offset_max = rect.anchored_position + math::vector2{(1.0f - rect.pivot.x()) * rect.size_delta.x(), (1.0f - rect.pivot.y()) * rect.size_delta.y()};

  const auto min_x = parent.position.x() + rect.anchor_min.x() * parent.size.x() + offset_min.x();
  const auto min_y = parent.position.y() + rect.anchor_min.y() * parent.size.y() + offset_min.y();
  const auto max_x = parent.position.x() + rect.anchor_max.x() * parent.size.x() + offset_max.x();
  const auto max_y = parent.position.y() + rect.anchor_max.y() * parent.size.y() + offset_max.y();

  return resolved_rect{math::vector2{min_x, min_y}, math::vector2{max_x - min_x, max_y - min_y}};
}

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_RECT_RESOLVE_HPP_
