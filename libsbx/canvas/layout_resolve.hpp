// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_CANVAS_LAYOUT_RESOLVE_HPP_
#define LIBSBX_CANVAS_LAYOUT_RESOLVE_HPP_

#include <utility>
#include <vector>

#include <libsbx/math/vector2.hpp>

#include <libsbx/scenes/scene.hpp>
#include <libsbx/scenes/node.hpp>

#include <libsbx/canvas/components.hpp>
#include <libsbx/canvas/rect_resolve.hpp>

namespace sbx::canvas {

struct axis_size {
  std::float_t min{0.0f};
  std::float_t preferred{0.0f};
  std::float_t flexible{0.0f};
}; // struct axis_size

[[nodiscard]] auto child_axis_size(scenes::node child, bool horizontal) -> axis_size;

[[nodiscard]] auto compute_preferred_size(scenes::scene& scene, scenes::node node, bool use_min) -> math::vector2;

[[nodiscard]] auto layout_horizontal_children(scenes::scene& scene, scenes::node node, const resolved_rect& rect) -> std::vector<std::pair<scenes::node, resolved_rect>>;

[[nodiscard]] auto layout_vertical_children(scenes::scene& scene, scenes::node node, const resolved_rect& rect) -> std::vector<std::pair<scenes::node, resolved_rect>>;

[[nodiscard]] auto layout_grid_children(scenes::scene& scene, scenes::node node, const resolved_rect& rect) -> std::vector<std::pair<scenes::node, resolved_rect>>;

} // namespace sbx::canvas

#endif // LIBSBX_CANVAS_LAYOUT_RESOLVE_HPP_
