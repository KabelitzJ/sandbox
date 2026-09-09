// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/path_corridor.hpp>

#include <algorithm>
#include <optional>

namespace sbx::physics {

auto corridor_reset(path_corridor& corridor, poly_ref ref, const math::vector3& pos) -> void {
  corridor.position = pos;
  corridor.target = pos;
  corridor.path.assign(1, ref);
}

auto corridor_set_corridor(path_corridor& corridor, const math::vector3& target, std::span<const poly_ref> path) -> void {
  corridor.target = target;
  corridor.path.assign(path.begin(), path.end());
}

[[nodiscard]] auto poly_contains_xz(const navmesh& mesh, poly_ref ref, const math::vector3& point) -> bool {
  const auto closest = closest_point_on_poly(mesh, ref, point);
  const auto dx = closest.x() - point.x();
  const auto dz = closest.z() - point.z();

  return (dx * dx + dz * dz) < 0.0001f;
}

[[nodiscard]] auto corridor_move_position(path_corridor& corridor, const navmesh& mesh, const math::vector3& new_pos) -> math::vector3 {
  if (corridor.path.empty()) {
    corridor.position = new_pos;

    return corridor.position;
  }

  constexpr auto look_ahead = std::size_t{4};
  const auto scan_count = std::min(look_ahead, corridor.path.size());

  auto found_index = std::optional<std::size_t>{};

  for (auto i = std::size_t{0}; i < scan_count; ++i) {
    if (poly_contains_xz(mesh, corridor.path[i], new_pos)) {
      found_index = i;
    }
  }

  if (found_index) {
    if (*found_index > 0) {
      corridor.path.erase(corridor.path.begin(), corridor.path.begin() + static_cast<std::ptrdiff_t>(*found_index));
    }

    corridor.position = closest_point_on_poly(mesh, corridor.path.front(), new_pos);
  } else {
    const auto nearest = find_nearest_poly(mesh, new_pos);

    corridor.position = (nearest != null_poly_ref) ? closest_point_on_poly(mesh, nearest, new_pos) : new_pos;
  }

  return corridor.position;
}

[[nodiscard]] auto corridor_find_corners(const path_corridor& corridor, const navmesh& mesh, std::size_t max_corners) -> std::vector<straight_path_point> {
  if (corridor.path.empty()) {
    return {};
  }

  auto corners = find_straight_path(mesh, corridor.position, corridor.target, std::span<const poly_ref>{corridor.path}, max_corners);

  while (!corners.empty() && math::vector3::distance_squared(corners.front().position, corridor.position) < 0.0001f) {
    corners.erase(corners.begin());
  }

  return corners;
}

} // namespace sbx::physics
