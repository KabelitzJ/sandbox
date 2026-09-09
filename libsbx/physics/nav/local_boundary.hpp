// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_LOCAL_BOUNDARY_HPP_
#define LIBSBX_PHYSICS_NAV_LOCAL_BOUNDARY_HPP_

#include <vector>

#include <libsbx/math/vector3.hpp>

#include <libsbx/physics/nav/navmesh.hpp>

namespace sbx::physics {

struct boundary_segment {
  math::vector3 start{};
  math::vector3 end{};
}; // struct boundary_segment

struct local_boundary {
  math::vector3 center{};
  std::vector<boundary_segment> segments{};
}; // struct local_boundary

auto local_boundary_update(local_boundary& boundary, const navmesh& mesh, poly_ref center_poly, const math::vector3& center, std::float_t range) -> void;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_LOCAL_BOUNDARY_HPP_
