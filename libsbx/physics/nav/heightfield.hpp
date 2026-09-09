// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_HEIGHTFIELD_HPP_
#define LIBSBX_PHYSICS_NAV_HEIGHTFIELD_HPP_

#include <array>
#include <cmath>
#include <cinttypes>
#include <deque>
#include <limits>
#include <memory_resource>
#include <span>
#include <utility>
#include <vector>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/volume.hpp>

namespace sbx::physics {

template<std::unsigned_integral Type>
struct bit_count {
  inline static constexpr auto value = sizeof(Type) * std::numeric_limits<std::uint8_t>::digits;
}; // struct bits_in

template<std::unsigned_integral Type>
inline constexpr auto bit_count_v = bit_count<Type>::value;

struct config {
  std::int32_t width;
  std::int32_t height;
  std::int32_t tile_size;
  std::int32_t border_size;
  std::float_t cell_size;
  std::float_t cell_height;
  math::volume bounds;
  std::float_t walkable_slope_angle;
  std::int32_t walkable_height;
  std::int32_t walkable_climb;
  std::int32_t walkable_radius;
  std::int32_t max_edge_length;
  std::float_t max_simplification_error;
  std::int32_t min_region_area;
  std::int32_t merge_region_area;
  std::int32_t max_verts_per_poly;
  std::float_t detail_sample_distance;
  std::float_t detail_sample_max_error;
}; // struct config

struct span_traits {
  inline static constexpr auto height_bits = 13u;
  inline static constexpr auto area_bits = 6u;
  inline static constexpr auto max_height_value = (1u << height_bits) - 1u;
  inline static constexpr auto pool_size = 2048u;
}; // struct span_traits

static_assert((span_traits::height_bits * 2 + span_traits::area_bits) == bit_count_v<std::uint32_t>, "Invalid layout for sbx::physics::span");

inline constexpr auto null_area = std::uint8_t{0};
inline constexpr auto walkable_area = std::uint8_t{63};
inline constexpr auto not_connected = std::uint8_t{0x3f};

struct span {
  std::uint32_t min : span_traits::height_bits;
  std::uint32_t max : span_traits::height_bits;
  std::uint32_t area : span_traits::area_bits;
  memory::observer_ptr<span> next;
}; // struct span

struct span_pool {
  std::array<span, span_traits::pool_size> buffer;
}; // struct span_pool

struct heightfield : utility::noncopyable {
  explicit heightfield(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
  : columns{resource}, pools{resource} {}

  std::int32_t width{0};
  std::int32_t height{0};
  math::volume bounds{};
  std::float_t cell_size{0.0f};
  std::float_t cell_height{0.0f};
  std::pmr::vector<memory::observer_ptr<span>> columns;
  std::pmr::deque<span_pool> pools;
  memory::observer_ptr<span> free_list{};
}; // struct heightfield

[[nodiscard]] constexpr auto direction_offset_x(std::int32_t direction) -> std::int32_t {
  constexpr auto offsets = std::array<std::int32_t, 4>{-1, 0, 1, 0};
  return offsets[direction & 0x03];
}

[[nodiscard]] constexpr auto direction_offset_z(std::int32_t direction) -> std::int32_t {
  constexpr auto offsets = std::array<std::int32_t, 4>{0, 1, 0, -1};
  return offsets[direction & 0x03];
}

[[nodiscard]] auto calc_grid_size(const math::volume& bounds, std::float_t cell_size) -> std::pair<std::int32_t, std::int32_t>;

[[nodiscard]] auto create_heightfield(std::int32_t width, std::int32_t height, const math::volume& bounds, std::float_t cell_size, std::float_t cell_height, std::pmr::memory_resource* resource = std::pmr::get_default_resource()) -> heightfield;

[[nodiscard]] auto is_walkable_triangle(const math::vector3& v0, const math::vector3& v1, const math::vector3& v2, std::float_t walkable_slope_angle) -> bool;

auto rasterize_triangle(heightfield& hf, const math::vector3& v0, const math::vector3& v1, const math::vector3& v2, std::uint8_t area, std::int32_t flag_merge_threshold = 1) -> void;

auto rasterize_triangles(heightfield& hf, std::span<const math::vector3> vertices, std::span<const std::uint32_t> indices, std::float_t walkable_slope_angle, std::int32_t flag_merge_threshold = 1) -> void;

auto filter_low_hanging_walkable_obstacles(heightfield& hf, std::int32_t walkable_climb) -> void;

auto filter_ledge_spans(heightfield& hf, std::int32_t walkable_height, std::int32_t walkable_climb) -> void;

auto filter_walkable_low_height_spans(heightfield& hf, std::int32_t walkable_height) -> void;

struct compact_cell_traits {
  inline static constexpr auto index_bits = 24u;
  inline static constexpr auto count_bits = 8u;
}; // struct compact_cell_traits

static_assert((compact_cell_traits::index_bits + compact_cell_traits::count_bits) == bit_count_v<std::uint32_t>, "Invalid layout for sbx::physics::compact_cell");

struct compact_cell {
  std::uint32_t index : compact_cell_traits::index_bits;
  std::uint32_t count : compact_cell_traits::count_bits;
}; // struct compact_cell

struct compact_span_traits {
  inline static constexpr auto connection_bits = 24u;
  inline static constexpr auto height_bits = 8u;
}; // struct compact_span_traits

static_assert((compact_span_traits::connection_bits + compact_span_traits::height_bits) == bit_count_v<std::uint32_t>, "Invalid layout for sbx::physics::compact_span");

struct compact_span {
  std::uint16_t min;
  std::uint16_t region_id;
  std::uint32_t connection : compact_span_traits::connection_bits;
  std::uint32_t height : compact_span_traits::height_bits;
}; // struct compact_span

[[nodiscard]] constexpr auto get_connection(const compact_span& value, std::int32_t direction) -> std::int32_t {
  const auto shift = static_cast<std::uint32_t>(direction) * 6u;
  return static_cast<std::int32_t>((value.connection >> shift) & 0x3fu);
}

constexpr auto set_connection(compact_span& value, std::int32_t direction, std::int32_t neighbor_index) -> void {
  const auto shift = static_cast<std::uint32_t>(direction) * 6u;
  value.connection = (value.connection & ~(0x3fu << shift)) | ((static_cast<std::uint32_t>(neighbor_index) & 0x3fu) << shift);
}

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_HEIGHTFIELD_HPP_
