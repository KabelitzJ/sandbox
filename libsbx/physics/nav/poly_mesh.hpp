// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_POLY_MESH_HPP_
#define LIBSBX_PHYSICS_NAV_POLY_MESH_HPP_

#include <cstdint>
#include <memory_resource>
#include <vector>

#include <libsbx/math/volume.hpp>

#include <libsbx/physics/nav/bake_arena.hpp>
#include <libsbx/physics/nav/contour.hpp>

namespace sbx::physics {

inline constexpr auto mesh_null_index = std::uint16_t{0xffff};

struct poly_mesh {
  explicit poly_mesh(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
  : verts{resource}, polys{resource}, regions{resource}, areas{resource}, flags{resource} {}

  std::pmr::vector<std::uint16_t> verts;
  std::pmr::vector<std::uint16_t> polys;
  std::pmr::vector<std::uint16_t> regions;
  std::pmr::vector<std::uint8_t> areas;
  std::pmr::vector<std::uint16_t> flags;
  std::int32_t num_verts{0};
  std::int32_t num_polys{0};
  std::int32_t max_verts_per_poly{0};
  math::volume bounds{};
  std::float_t cell_size{0.0f};
  std::float_t cell_height{0.0f};
  std::int32_t border_size{0};
  std::float_t max_edge_error{0.0f};
}; // struct poly_mesh

[[nodiscard]] auto build_poly_mesh(const contour_set& contours, std::int32_t max_verts_per_poly, bake_arena& arena) -> poly_mesh;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_POLY_MESH_HPP_
