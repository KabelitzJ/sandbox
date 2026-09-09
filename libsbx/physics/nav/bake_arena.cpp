// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/bake_arena.hpp>

namespace sbx::physics {

bake_arena::bake_arena(std::size_t initial_permanent_bytes, std::size_t initial_temp_bytes)
: _permanent_buffer(initial_permanent_bytes),
  _permanent{_permanent_buffer.data(), _permanent_buffer.size(), std::pmr::new_delete_resource()},
  _temp_buffer(initial_temp_bytes),
  _temp{_temp_buffer.data(), _temp_buffer.size(), std::pmr::new_delete_resource()} {}

} // namespace sbx::physics
