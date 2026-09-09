// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_CONTOUR_HPP_
#define LIBSBX_PHYSICS_NAV_CONTOUR_HPP_

#include <cstdint>
#include <memory_resource>
#include <vector>

#include <libsbx/math/volume.hpp>

#include <libsbx/physics/nav/bake_arena.hpp>
#include <libsbx/physics/nav/compact_heightfield.hpp>

namespace sbx::physics {

inline constexpr auto border_vertex = std::int32_t{0x10000};
inline constexpr auto area_border = std::int32_t{0x20000};
inline constexpr auto contour_region_mask = std::int32_t{0xffff};
inline constexpr auto tess_wall_edges = std::int32_t{0x01};
inline constexpr auto tess_area_edges = std::int32_t{0x02};

struct contour_vertex {
  std::int32_t x{0};
  std::int32_t y{0};
  std::int32_t z{0};
  std::int32_t flags{0};
}; // struct contour_vertex

struct contour {
  explicit contour(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
  : verts{resource}, raw_verts{resource} {}

  std::pmr::vector<contour_vertex> verts;
  std::pmr::vector<contour_vertex> raw_verts;
  std::uint16_t region_id{0};
  std::uint8_t area{0};
}; // struct contour

struct contour_set {
  explicit contour_set(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
  : contours{resource} {}

  std::pmr::vector<contour> contours;
  math::volume bounds{};
  std::float_t cell_size{0.0f};
  std::float_t cell_height{0.0f};
  std::int32_t width{0};
  std::int32_t height{0};
  std::int32_t border_size{0};
  std::float_t max_error{0.0f};
}; // struct contour_set

[[nodiscard]] auto build_contours(const compact_heightfield& chf, std::float_t max_error, std::int32_t max_edge_length, bake_arena& arena, std::int32_t build_flags = tess_wall_edges) -> contour_set;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_CONTOUR_HPP_
