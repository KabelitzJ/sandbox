// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/path_corridor.hpp>

#include <algorithm>
#include <optional>

namespace sbx::physics {

auto corridor_reset(path_corridor& corridor, poly_reference reference, const math::vector3& position) -> void {
  corridor.position = position;
  corridor.target = position;
  corridor.path.assign(1, reference);
}

auto corridor_set_corridor(path_corridor& corridor, const math::vector3& target, std::span<const poly_reference> path) -> void {
  corridor.target = target;
  corridor.path.assign(path.begin(), path.end());
}

[[nodiscard]] auto merge_corridor_start_moved(std::vector<poly_reference>& path, const std::vector<poly_reference>& visited) -> bool {
  auto furthest_path = std::optional<std::size_t>{};
  auto furthest_visited = std::optional<std::size_t>{};

  for (auto pi = path.size(); pi > 0 && !furthest_path; --pi) {
    const auto i = pi - 1;

    for (auto vi = visited.size(); vi > 0; --vi) {
      const auto j = vi - 1;

      if (path[i] == visited[j]) {
        furthest_path = i;
        furthest_visited = j;

        break;
      }
    }
  }

  if (!furthest_path || !furthest_visited) {
    return false;
  }

  auto merged = std::vector<poly_reference>{};
  merged.reserve((visited.size() - *furthest_visited) + (path.size() - (*furthest_path + 1)));

  for (auto vi = visited.size(); vi > *furthest_visited; --vi) {
    merged.push_back(visited[vi - 1]);
  }

  for (auto i = *furthest_path + 1; i < path.size(); ++i) {
    merged.push_back(path[i]);
  }

  path = std::move(merged);

  return true;
}

[[nodiscard]] auto corridor_move_position(path_corridor& corridor, const navmesh& mesh, const math::vector3& new_position) -> math::vector3 {
  if (corridor.path.empty()) {
    corridor.position = new_position;

    return corridor.position;
  }

  const auto walk = move_along_surface(mesh, corridor.path.front(), corridor.position, new_position);

  if (merge_corridor_start_moved(corridor.path, walk.visited)) {
    const auto height = sample_height_on_poly(mesh, corridor.path.front(), walk.position);
    corridor.position = math::vector3{walk.position.x(), height, walk.position.z()};

    return corridor.position;
  }

  const auto nearest = find_nearest_poly(mesh, new_position);

  if (nearest == null_poly_reference) {
    corridor.position = new_position;

    return corridor.position;
  }

  const auto end_ref = find_nearest_poly(mesh, corridor.target);
  const auto result = (end_ref != null_poly_reference) ? find_path(mesh, nearest, end_ref, new_position, corridor.target) : path_result{};

  corridor.path = result.success ? result.polys : std::vector<poly_reference>{nearest};
  corridor.position = closest_point_on_poly(mesh, corridor.path.front(), new_position);

  return corridor.position;
}

[[nodiscard]] auto corridor_find_corners(const path_corridor& corridor, const navmesh& mesh, std::size_t max_corners) -> std::vector<straight_path_point> {
  if (corridor.path.empty()) {
    return {};
  }

  auto corners = find_straight_path(mesh, corridor.position, corridor.target, std::span<const poly_reference>{corridor.path}, max_corners);

  while (!corners.empty() && math::vector3::distance_squared(corners.front().position, corridor.position) < 0.0001f) {
    corners.erase(corners.begin());
  }

  return corners;
}

} // namespace sbx::physics
