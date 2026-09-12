// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/local_boundary.hpp>

#include <algorithm>
#include <deque>
#include <unordered_set>
#include <vector>

namespace sbx::physics {

[[nodiscard]] auto distance_point_segment_sqr_xz(const math::vector3& point, const math::vector3& a, const math::vector3& b) -> std::float_t {
  const auto abx = b.x() - a.x();
  const auto abz = b.z() - a.z();
  const auto apx = point.x() - a.x();
  const auto apz = point.z() - a.z();

  const auto ab_length_squared = abx * abx + abz * abz;

  auto t = ab_length_squared > 0.0f ? (apx * abx + apz * abz) / ab_length_squared : 0.0f;
  t = std::clamp(t, 0.0f, 1.0f);

  const auto cx = a.x() + abx * t;
  const auto cz = a.z() + abz * t;

  const auto dx = point.x() - cx;
  const auto dz = point.z() - cz;

  return dx * dx + dz * dz;
}

auto insert_boundary_segment(local_boundary& boundary, std::vector<std::float_t>& distances, std::size_t max_segments, std::float_t distance_squared, const boundary_segment& segment) -> void {
  if (boundary.segments.size() >= max_segments && distance_squared >= distances.back()) {
    return;
  }

  auto insert_at = boundary.segments.size();

  while (insert_at > 0 && distances[insert_at - 1] > distance_squared) {
    --insert_at;
  }

  distances.insert(distances.begin() + static_cast<std::ptrdiff_t>(insert_at), distance_squared);
  boundary.segments.insert(boundary.segments.begin() + static_cast<std::ptrdiff_t>(insert_at), segment);

  if (boundary.segments.size() > max_segments) {
    distances.pop_back();
    boundary.segments.pop_back();
  }
}

auto local_boundary_update(local_boundary& boundary, const navmesh& mesh, poly_reference center_poly, const math::vector3& center, std::float_t range) -> void {
  boundary.center = center;
  boundary.segments.clear();

  if (center_poly == null_poly_reference) {
    return;
  }

  constexpr auto max_segments = std::size_t{8};

  auto distances = std::vector<std::float_t>{};
  distances.reserve(max_segments);

  auto visited = std::unordered_set<poly_reference>{};
  auto queue = std::deque<poly_reference>{};

  visited.insert(center_poly);
  queue.push_back(center_poly);

  while (!queue.empty()) {
    const auto current = queue.front();
    queue.pop_front();

    const auto& poly = mesh.polys[poly_ref_to_index(current)];
    const auto count = poly.verts.size();

    for (auto i = std::size_t{0}; i < count; ++i) {
      const auto neighbor = poly.neighbors[i];

      if (neighbor == null_poly_reference) {
        const auto segment = boundary_segment{mesh.verts[poly.verts[i]], mesh.verts[poly.verts[(i + 1u) % count]]};
        const auto distance_squared = distance_point_segment_sqr_xz(center, segment.start, segment.end);

        insert_boundary_segment(boundary, distances, max_segments, distance_squared, segment);
      } else if (!visited.contains(neighbor)) {
        const auto neighbor_center = poly_center(mesh, neighbor);
        const auto dx = neighbor_center.x() - center.x();
        const auto dz = neighbor_center.z() - center.z();

        if ((dx * dx + dz * dz) <= range * range) {
          visited.insert(neighbor);
          queue.push_back(neighbor);
        }
      }
    }
  }
}

} // namespace sbx::physics
