// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/navmesh_query.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace sbx::physics {

[[nodiscard]] auto distance_xz(const math::vector3& a, const math::vector3& b) -> std::float_t {
  const auto dx = a.x() - b.x();
  const auto dz = a.z() - b.z();

  return std::sqrt(dx * dx + dz * dz);
}

[[nodiscard]] auto find_path(const navmesh& mesh, poly_ref start, poly_ref end, const math::vector3& start_pos, const math::vector3& end_pos) -> path_result {
  if (start == null_poly_ref || end == null_poly_ref) {
    return path_result{};
  }

  if (start == end) {
    return path_result{std::vector<poly_ref>{start}, true};
  }

  struct open_entry {
    std::float_t f_score;
    poly_ref ref;
  };

  const auto compare = [](const open_entry& a, const open_entry& b) {
    return a.f_score > b.f_score;
  };

  auto open = std::priority_queue<open_entry, std::vector<open_entry>, decltype(compare)>{compare};

  auto g_score = std::unordered_map<poly_ref, std::float_t>{};
  auto came_from = std::unordered_map<poly_ref, poly_ref>{};
  auto closed = std::unordered_set<poly_ref>{};

  const auto heuristic = [&](poly_ref ref) {
    return distance_xz(poly_center(mesh, ref), end_pos);
  };

  g_score[start] = 0.0f;
  open.push(open_entry{heuristic(start), start});

  while (!open.empty()) {
    const auto current = open.top().ref;
    open.pop();

    if (current == end) {
      auto polys = std::vector<poly_ref>{current};
      auto it = came_from.find(current);

      while (it != came_from.end()) {
        polys.push_back(it->second);
        it = came_from.find(it->second);
      }

      std::reverse(polys.begin(), polys.end());

      return path_result{std::move(polys), true};
    }

    if (closed.contains(current)) {
      continue;
    }

    closed.insert(current);

    const auto& poly = mesh.polys[poly_ref_to_index(current)];
    const auto current_pos = (current == start) ? start_pos : poly_center(mesh, current);

    for (const auto neighbor : poly.neighbors) {
      if (neighbor == null_poly_ref || closed.contains(neighbor)) {
        continue;
      }

      const auto edge_cost = distance_xz(current_pos, poly_center(mesh, neighbor));
      const auto tentative_g = g_score[current] + edge_cost;

      const auto existing = g_score.find(neighbor);

      if (existing == g_score.end() || tentative_g < existing->second) {
        g_score[neighbor] = tentative_g;
        came_from[neighbor] = current;
        open.push(open_entry{tentative_g + heuristic(neighbor), neighbor});
      }
    }
  }

  return path_result{};
}

[[nodiscard]] auto triangle_area_2d(const math::vector3& a, const math::vector3& b, const math::vector3& c) -> std::float_t {
  const auto abx = b.x() - a.x();
  const auto abz = b.z() - a.z();
  const auto acx = c.x() - a.x();
  const auto acz = c.z() - a.z();

  return acx * abz - abx * acz;
}

[[nodiscard]] auto vertices_close(const math::vector3& a, const math::vector3& b) -> bool {
  constexpr auto threshold = 1.0f / (16384.0f * 16384.0f);

  return math::vector3::distance_squared(a, b) < threshold;
}

[[nodiscard]] auto append_straight_vertex(std::vector<straight_path_point>& result, const math::vector3& position, poly_ref ref, std::size_t max_points) -> bool {
  if (!result.empty() && vertices_close(result.back().position, position)) {
    result.back().ref = ref;

    return true;
  }

  result.push_back(straight_path_point{position, ref});

  return result.size() < max_points;
}

[[nodiscard]] auto find_straight_path(const navmesh& mesh, const math::vector3& start_pos, const math::vector3& end_pos, std::span<const poly_ref> poly_path, std::size_t max_points) -> std::vector<straight_path_point> {
  auto result = std::vector<straight_path_point>{};

  if (poly_path.empty() || max_points == 0) {
    return result;
  }

  const auto closest_start = closest_point_on_poly(mesh, poly_path[0], start_pos);
  const auto closest_end = closest_point_on_poly(mesh, poly_path.back(), end_pos);

  if (!append_straight_vertex(result, closest_start, poly_path[0], max_points)) {
    return result;
  }

  if (poly_path.size() == 1) {
    append_straight_vertex(result, closest_end, null_poly_ref, max_points);

    return result;
  }

  auto portal_apex = closest_start;
  auto portal_left = portal_apex;
  auto portal_right = portal_apex;

  auto apex_index = std::size_t{0};
  auto left_index = std::size_t{0};
  auto right_index = std::size_t{0};

  auto left_poly_ref = poly_path[0];
  auto right_poly_ref = poly_path[0];

  const auto path_size = poly_path.size();

  for (auto i = std::size_t{0}; i < path_size; ++i) {
    auto left = math::vector3{};
    auto right = math::vector3{};

    if (i + 1 < path_size) {
      if (!poly_portal_points(mesh, poly_path[i], poly_path[i + 1], left, right)) {
        break;
      }
    } else {
      left = closest_end;
      right = closest_end;
    }

    if (triangle_area_2d(portal_apex, portal_right, right) <= 0.0f) {
      if (vertices_close(portal_apex, portal_right) || triangle_area_2d(portal_apex, portal_left, right) > 0.0f) {
        portal_right = right;
        right_poly_ref = (i + 1 < path_size) ? poly_path[i + 1] : null_poly_ref;
        right_index = i;
      } else {
        portal_apex = portal_left;
        apex_index = left_index;

        if (!append_straight_vertex(result, portal_apex, left_poly_ref, max_points)) {
          return result;
        }

        portal_left = portal_apex;
        portal_right = portal_apex;
        left_index = apex_index;
        right_index = apex_index;

        i = apex_index;

        continue;
      }
    }

    if (triangle_area_2d(portal_apex, portal_left, left) >= 0.0f) {
      if (vertices_close(portal_apex, portal_left) || triangle_area_2d(portal_apex, portal_right, left) < 0.0f) {
        portal_left = left;
        left_poly_ref = (i + 1 < path_size) ? poly_path[i + 1] : null_poly_ref;
        left_index = i;
      } else {
        portal_apex = portal_right;
        apex_index = right_index;

        if (!append_straight_vertex(result, portal_apex, right_poly_ref, max_points)) {
          return result;
        }

        portal_left = portal_apex;
        portal_right = portal_apex;
        left_index = apex_index;
        right_index = apex_index;

        i = apex_index;

        continue;
      }
    }
  }

  append_straight_vertex(result, closest_end, null_poly_ref, max_points);

  return result;
}

} // namespace sbx::physics
