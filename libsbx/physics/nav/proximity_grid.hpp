// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_PROXIMITY_GRID_HPP_
#define LIBSBX_PHYSICS_NAV_PROXIMITY_GRID_HPP_

#include <cstdint>
#include <vector>

#include <libsbx/math/vector3.hpp>

#include <libsbx/containers/dense_map.hpp>

#include <libsbx/scenes/node.hpp>

namespace sbx::physics {

class proximity_grid {

public:

  explicit proximity_grid(std::float_t cell_size = 2.0f)
  : _cell_size{cell_size} {}

  auto clear() -> void;

  auto insert(scenes::node node, const math::vector3& position) -> void;

  auto query(const math::vector3& center, std::float_t radius, std::vector<scenes::node>& out) const -> void;

private:

  [[nodiscard]] auto cell_coord(std::float_t value) const -> std::int32_t;

  [[nodiscard]] auto make_key(std::int32_t cx, std::int32_t cz) const -> std::int64_t;

  [[nodiscard]] auto cell_key(const math::vector3& position) const -> std::int64_t;

  std::float_t _cell_size;
  containers::dense_map<std::int64_t, std::vector<scenes::node>> _cells{};

}; // class proximity_grid

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_PROXIMITY_GRID_HPP_
