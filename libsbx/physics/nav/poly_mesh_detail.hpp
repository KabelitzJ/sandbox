// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_POLY_MESH_DETAIL_HPP_
#define LIBSBX_PHYSICS_NAV_POLY_MESH_DETAIL_HPP_

#include <array>
#include <cstdint>
#include <memory_resource>
#include <vector>

#include <libsbx/physics/nav/bake_arena.hpp>
#include <libsbx/physics/nav/compact_heightfield.hpp>
#include <libsbx/physics/nav/poly_mesh.hpp>

namespace sbx::physics {

struct poly_mesh_detail {
  explicit poly_mesh_detail(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
  : verts{resource}, tris{resource}, meshes{resource} {}

  std::pmr::vector<std::array<std::float_t, 3>> verts;
  std::pmr::vector<std::array<std::uint32_t, 3>> tris;
  std::pmr::vector<std::array<std::uint32_t, 4>> meshes;
}; // struct poly_mesh_detail

[[nodiscard]] auto build_poly_mesh_detail(const poly_mesh& pmesh, const compact_heightfield& chf, std::float_t sample_distance, std::float_t sample_max_error, bake_arena& arena) -> poly_mesh_detail;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_POLY_MESH_DETAIL_HPP_
