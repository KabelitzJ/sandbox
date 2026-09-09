// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/proximity_grid.hpp>

#include <cmath>

namespace sbx::physics {

auto proximity_grid::clear() -> void {
  _cells.clear();
}

auto proximity_grid::insert(scenes::node node, const math::vector3& position) -> void {
  const auto key = cell_key(position);
  const auto result = _cells.emplace(key, std::vector<scenes::node>{});

  result.first->second.push_back(node);
}

auto proximity_grid::query(const math::vector3& center, std::float_t radius, std::vector<scenes::node>& out) const -> void {
  const auto min_x = cell_coord(center.x() - radius);
  const auto max_x = cell_coord(center.x() + radius);
  const auto min_z = cell_coord(center.z() - radius);
  const auto max_z = cell_coord(center.z() + radius);

  for (auto cx = min_x; cx <= max_x; ++cx) {
    for (auto cz = min_z; cz <= max_z; ++cz) {
      const auto entry = _cells.find(make_key(cx, cz));

      if (entry != _cells.end()) {
        for (const auto& node : entry->second) {
          out.push_back(node);
        }
      }
    }
  }
}

[[nodiscard]] auto proximity_grid::cell_coord(std::float_t value) const -> std::int32_t {
  return static_cast<std::int32_t>(std::floor(value / _cell_size));
}

[[nodiscard]] auto proximity_grid::make_key(std::int32_t cx, std::int32_t cz) const -> std::int64_t {
  return (static_cast<std::int64_t>(cx) << 32) | static_cast<std::uint32_t>(cz);
}

[[nodiscard]] auto proximity_grid::cell_key(const math::vector3& position) const -> std::int64_t {
  return make_key(cell_coord(position.x()), cell_coord(position.z()));
}

} // namespace sbx::physics
