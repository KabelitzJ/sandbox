// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/local_boundary.hpp>

#include <deque>
#include <unordered_set>

namespace sbx::physics {

auto local_boundary_update(local_boundary& boundary, const navmesh& mesh, poly_ref center_poly, const math::vector3& center, std::float_t range) -> void {
  boundary.center = center;
  boundary.segments.clear();

  if (center_poly == null_poly_ref) {
    return;
  }

  constexpr auto max_segments = std::size_t{8};

  auto visited = std::unordered_set<poly_ref>{};
  auto queue = std::deque<poly_ref>{};

  visited.insert(center_poly);
  queue.push_back(center_poly);

  while (!queue.empty() && boundary.segments.size() < max_segments) {
    const auto current = queue.front();
    queue.pop_front();

    const auto& poly = mesh.polys[poly_ref_to_index(current)];
    const auto count = poly.verts.size();

    for (auto i = std::size_t{0}; i < count; ++i) {
      const auto neighbor = poly.neighbors[i];

      if (neighbor == null_poly_ref) {
        if (boundary.segments.size() >= max_segments) {
          break;
        }

        boundary.segments.push_back(boundary_segment{mesh.verts[poly.verts[i]], mesh.verts[poly.verts[(i + 1u) % count]]});
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
