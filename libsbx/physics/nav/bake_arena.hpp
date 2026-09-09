// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_BAKE_ARENA_HPP_
#define LIBSBX_PHYSICS_NAV_BAKE_ARENA_HPP_

#include <cstddef>
#include <memory_resource>
#include <vector>

namespace sbx::physics {

class bake_arena {

public:

  inline static constexpr auto default_permanent_bytes = std::size_t{1u << 20};
  inline static constexpr auto default_temp_bytes = std::size_t{1u << 18};

  explicit bake_arena(std::size_t initial_permanent_bytes = default_permanent_bytes, std::size_t initial_temp_bytes = default_temp_bytes);

  bake_arena(const bake_arena&) = delete;
  bake_arena(bake_arena&&) = delete;

  auto operator=(const bake_arena&) -> bake_arena& = delete;
  auto operator=(bake_arena&&) -> bake_arena& = delete;

  [[nodiscard]] auto permanent() noexcept -> std::pmr::memory_resource* {
    return &_permanent;
  }

  [[nodiscard]] auto temp() noexcept -> std::pmr::memory_resource* {
    return &_temp;
  }

  auto reset_temp() -> void {
    _temp.release();
  }

private:

  std::vector<std::byte> _permanent_buffer;
  std::pmr::monotonic_buffer_resource _permanent;

  std::vector<std::byte> _temp_buffer;
  std::pmr::monotonic_buffer_resource _temp;

}; // class bake_arena

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_BAKE_ARENA_HPP_
