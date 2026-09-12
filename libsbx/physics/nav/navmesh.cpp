// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/navmesh.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace sbx::physics {

[[nodiscard]] auto build_runtime_navmesh(const poly_mesh& pmesh, const poly_mesh_detail& dmesh, std::float_t walkable_climb) -> navmesh {
  auto mesh = navmesh{};
  mesh.bounds = pmesh.bounds;
  mesh.walkable_climb = walkable_climb;

  mesh.verts.reserve(static_cast<std::size_t>(pmesh.num_verts));

  for (auto i = std::int32_t{0}; i < pmesh.num_verts; ++i) {
    const auto wx = pmesh.bounds.min().x() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(i) * 3u + 0u]) * pmesh.cell_size;
    const auto wy = pmesh.bounds.min().y() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(i) * 3u + 1u]) * pmesh.cell_height;
    const auto wz = pmesh.bounds.min().z() + static_cast<std::float_t>(pmesh.verts[static_cast<std::size_t>(i) * 3u + 2u]) * pmesh.cell_size;

    mesh.verts.push_back(math::vector3{wx, wy, wz});
  }

  mesh.detail_verts.reserve(dmesh.verts.size());

  for (const auto& v : dmesh.verts) {
    mesh.detail_verts.push_back(math::vector3{v[0], v[1], v[2]});
  }

  mesh.detail_tris.assign(dmesh.tris.begin(), dmesh.tris.end());

  const auto nvp = pmesh.max_verts_per_poly;

  mesh.polys.reserve(static_cast<std::size_t>(pmesh.num_polys));

  for (auto i = std::int32_t{0}; i < pmesh.num_polys; ++i) {
    const auto poly = &pmesh.polys[static_cast<std::size_t>(i) * static_cast<std::size_t>(nvp) * 2u];

    auto value = nav_poly{};

    for (auto j = std::int32_t{0}; j < nvp; ++j) {
      if (poly[j] == mesh_null_index) {
        break;
      }

      value.verts.push_back(poly[j]);

      const auto neighbor = poly[nvp + j];
      value.neighbors.push_back(neighbor == mesh_null_index ? null_poly_reference : poly_index_to_ref(static_cast<std::size_t>(neighbor)));
    }

    value.area = pmesh.areas[static_cast<std::size_t>(i)];
    value.flags = pmesh.flags[static_cast<std::size_t>(i)];

    if (static_cast<std::size_t>(i) < dmesh.meshes.size()) {
      const auto& entry = dmesh.meshes[static_cast<std::size_t>(i)];
      value.detail_vert_base = entry[0];
      value.detail_vert_count = entry[1];
      value.detail_tri_base = entry[2];
      value.detail_tri_count = entry[3];
    }

    mesh.polys.push_back(std::move(value));
  }

  return mesh;
}

[[nodiscard]] auto poly_center(const navmesh& mesh, poly_reference reference) -> math::vector3 {
  const auto& poly = mesh.polys[poly_ref_to_index(reference)];

  auto center = math::vector3::zero;

  for (const auto v : poly.verts) {
    center = center + mesh.verts[v];
  }

  return center / static_cast<std::float_t>(poly.verts.size());
}

[[nodiscard]] auto barycentric_xz(const math::vector3& point, const math::vector3& a, const math::vector3& b, const math::vector3& c, std::float_t& u, std::float_t& v, std::float_t& w) -> bool {
  const auto ab_x = b.x() - a.x();
  const auto ab_z = b.z() - a.z();
  const auto ac_x = c.x() - a.x();
  const auto ac_z = c.z() - a.z();
  const auto ap_x = point.x() - a.x();
  const auto ap_z = point.z() - a.z();

  const auto denom = ab_x * ac_z - ac_x * ab_z;

  if (std::abs(denom) < 1e-9f) {
    return false;
  }

  const auto inv_denom = 1.0f / denom;

  v = (ap_x * ac_z - ac_x * ap_z) * inv_denom;
  w = (ab_x * ap_z - ap_x * ab_z) * inv_denom;
  u = 1.0f - v - w;

  return true;
}

[[nodiscard]] auto interpolate_poly_height(const navmesh& mesh, poly_reference reference, const math::vector3& point) -> std::float_t {
  const auto& poly = mesh.polys[poly_ref_to_index(reference)];

  for (auto i = std::uint32_t{0}; i < poly.detail_tri_count; ++i) {
    const auto& tri = mesh.detail_tris[poly.detail_tri_base + i];
    const auto& a = mesh.detail_verts[tri[0]];
    const auto& b = mesh.detail_verts[tri[1]];
    const auto& c = mesh.detail_verts[tri[2]];

    auto u = 0.0f;
    auto v = 0.0f;
    auto w = 0.0f;

    if (barycentric_xz(point, a, b, c, u, v, w) && u >= -0.001f && v >= -0.001f && w >= -0.001f) {
      return u * a.y() + v * b.y() + w * c.y();
    }
  }

  const auto count = poly.verts.size();

  if (count >= 3) {
    const auto& v0 = mesh.verts[poly.verts[0]];

    for (auto i = std::size_t{1}; i + 1 < count; ++i) {
      const auto& v1 = mesh.verts[poly.verts[i]];
      const auto& v2 = mesh.verts[poly.verts[i + 1u]];

      auto u = 0.0f;
      auto v = 0.0f;
      auto w = 0.0f;

      if (barycentric_xz(point, v0, v1, v2, u, v, w) && u >= -0.001f && v >= -0.001f && w >= -0.001f) {
        return u * v0.y() + v * v1.y() + w * v2.y();
      }
    }
  }

  return poly_center(mesh, reference).y();
}

[[nodiscard]] auto closest_point_on_poly(const navmesh& mesh, poly_reference reference, const math::vector3& point) -> math::vector3 {
  const auto& poly = mesh.polys[poly_ref_to_index(reference)];
  const auto count = poly.verts.size();

  auto all_non_negative = true;
  auto all_non_positive = true;

  auto best_distance = std::numeric_limits<std::float_t>::max();
  auto best_point = mesh.verts[poly.verts[0]];

  for (auto i = std::size_t{0}; i < count; ++i) {
    const auto& a = mesh.verts[poly.verts[i]];
    const auto& b = mesh.verts[poly.verts[(i + 1u) % count]];

    const auto edge_x = b.x() - a.x();
    const auto edge_z = b.z() - a.z();
    const auto to_point_x = point.x() - a.x();
    const auto to_point_z = point.z() - a.z();

    const auto cross = edge_x * to_point_z - edge_z * to_point_x;

    if (cross < 0.0f) {
      all_non_negative = false;
    }

    if (cross > 0.0f) {
      all_non_positive = false;
    }

    const auto edge_length_squared = edge_x * edge_x + edge_z * edge_z;
    auto t = edge_length_squared > 0.0f ? (to_point_x * edge_x + to_point_z * edge_z) / edge_length_squared : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    const auto closest_x = a.x() + edge_x * t;
    const auto closest_z = a.z() + edge_z * t;

    const auto dx = point.x() - closest_x;
    const auto dz = point.z() - closest_z;
    const auto distance = dx * dx + dz * dz;

    if (distance < best_distance) {
      best_distance = distance;
      best_point = math::vector3{closest_x, a.y(), closest_z};
    }
  }

  if (all_non_negative || all_non_positive) {
    return math::vector3{point.x(), interpolate_poly_height(mesh, reference, point), point.z()};
  }

  return best_point;
}

[[nodiscard]] auto sample_height_on_poly(const navmesh& mesh, poly_reference reference, const math::vector3& point) -> std::float_t {
  return interpolate_poly_height(mesh, reference, point);
}

[[nodiscard]] auto poly_edge_midpoint(const navmesh& mesh, poly_reference reference, std::uint32_t edge_index) -> math::vector3 {
  const auto& poly = mesh.polys[poly_ref_to_index(reference)];
  const auto count = poly.verts.size();

  const auto& a = mesh.verts[poly.verts[edge_index]];
  const auto& b = mesh.verts[poly.verts[(static_cast<std::size_t>(edge_index) + 1u) % count]];

  return (a + b) * 0.5f;
}

[[nodiscard]] auto find_nearest_poly(const navmesh& mesh, const math::vector3& point) -> poly_reference {
  auto best_ref = null_poly_reference;
  auto best_distance = std::numeric_limits<std::float_t>::max();

  for (auto i = std::size_t{0}; i < mesh.polys.size(); ++i) {
    const auto reference = poly_index_to_ref(i);
    const auto closest = closest_point_on_poly(mesh, reference, point);

    const auto over_poly = closest.x() == point.x() && closest.z() == point.z();

    auto distance = std::float_t{0.0f};

    if (over_poly) {
      const auto vertical = std::abs(point.y() - closest.y()) - mesh.walkable_climb;
      distance = vertical > 0.0f ? vertical * vertical : 0.0f;
    } else {
      const auto dx = point.x() - closest.x();
      const auto dy = point.y() - closest.y();
      const auto dz = point.z() - closest.z();
      distance = dx * dx + dy * dy + dz * dz;
    }

    if (distance < best_distance) {
      best_distance = distance;
      best_ref = reference;
    }
  }

  return best_ref;
}

[[nodiscard]] auto poly_portal_points(const navmesh& mesh, poly_reference from, poly_reference to, math::vector3& left, math::vector3& right) -> bool {
  const auto& poly = mesh.polys[poly_ref_to_index(from)];
  const auto count = poly.verts.size();

  for (auto i = std::size_t{0}; i < count; ++i) {
    if (poly.neighbors[i] == to) {
      left = mesh.verts[poly.verts[i]];
      right = mesh.verts[poly.verts[(i + 1u) % count]];

      return true;
    }
  }

  return false;
}

} // namespace sbx::physics
