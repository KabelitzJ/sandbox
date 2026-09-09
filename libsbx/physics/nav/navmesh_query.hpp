// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_NAVMESH_QUERY_HPP_
#define LIBSBX_PHYSICS_NAV_NAVMESH_QUERY_HPP_

#include <cstddef>
#include <span>
#include <vector>

#include <libsbx/math/vector3.hpp>

#include <libsbx/physics/nav/navmesh.hpp>

namespace sbx::physics {

struct path_result {
  std::vector<poly_ref> polys{};
  bool success{false};
}; // struct path_result

[[nodiscard]] auto find_path(const navmesh& mesh, poly_ref start, poly_ref end, const math::vector3& start_pos, const math::vector3& end_pos) -> path_result;

struct straight_path_point {
  math::vector3 position{};
  poly_ref ref{null_poly_ref};
}; // struct straight_path_point

[[nodiscard]] auto find_straight_path(const navmesh& mesh, const math::vector3& start_pos, const math::vector3& end_pos, std::span<const poly_ref> poly_path, std::size_t max_points) -> std::vector<straight_path_point>;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_NAVMESH_QUERY_HPP_
