// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_NAVMESH_HPP_
#define LIBSBX_PHYSICS_NAV_NAVMESH_HPP_

#include <cmath>
#include <cstdint>
#include <vector>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/physics/nav/poly_mesh.hpp>

namespace sbx::physics {

using poly_reference = std::uint32_t;

inline constexpr auto null_poly_reference = poly_reference{0};

[[nodiscard]] constexpr auto poly_ref_to_index(poly_reference reference) -> std::size_t {
  return static_cast<std::size_t>(reference - 1u);
}

[[nodiscard]] constexpr auto poly_index_to_ref(std::size_t index) -> poly_reference {
  return static_cast<poly_reference>(index) + 1u;
}

struct nav_poly {
  std::vector<std::uint16_t> verts{};
  std::vector<poly_reference> neighbors{};
  std::uint8_t area{walkable_area};
  std::uint16_t flags{0};
}; // struct nav_poly

struct navmesh {
  std::vector<math::vector3> verts{};
  std::vector<nav_poly> polys{};
  math::volume bounds{};
  std::float_t walkable_climb{0.0f};
}; // struct navmesh

[[nodiscard]] auto build_runtime_navmesh(const poly_mesh& pmesh, std::float_t walkable_climb) -> navmesh;

[[nodiscard]] auto poly_center(const navmesh& mesh, poly_reference reference) -> math::vector3;

[[nodiscard]] auto closest_point_on_poly(const navmesh& mesh, poly_reference reference, const math::vector3& point) -> math::vector3;

[[nodiscard]] auto sample_height_on_poly(const navmesh& mesh, poly_reference reference, const math::vector3& point) -> std::float_t;

[[nodiscard]] auto poly_edge_midpoint(const navmesh& mesh, poly_reference reference, std::uint32_t edge_index) -> math::vector3;

[[nodiscard]] auto find_nearest_poly(const navmesh& mesh, const math::vector3& point) -> poly_reference;

[[nodiscard]] auto poly_portal_points(const navmesh& mesh, poly_reference from, poly_reference to, math::vector3& left, math::vector3& right) -> bool;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_NAVMESH_HPP_
