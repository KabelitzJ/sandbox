// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_NAV_AGENT_HPP_
#define LIBSBX_PHYSICS_NAV_NAV_AGENT_HPP_

#include <cstdint>

#include <libsbx/math/vector3.hpp>

#include <libsbx/physics/nav/local_boundary.hpp>
#include <libsbx/physics/nav/path_corridor.hpp>

namespace sbx::physics {

enum class nav_agent_state : std::uint8_t {
  idle,
  moving,
  target_unreachable
}; // enum class nav_agent_state

struct nav_agent {
  std::float_t radius{0.3f};
  std::float_t height{1.8f};
  std::float_t base_offset{height * 0.5f};
  std::float_t max_acceleration{8.0f};
  std::float_t max_speed{3.5f};
  std::float_t collision_query_range{6.0f};
  std::float_t path_optimization_range{15.0f};
  std::float_t separation_weight{2.0f};
  bool obstacle_avoidance_enabled{true};
  bool separation_enabled{true};

  nav_agent_state state{nav_agent_state::idle};
  math::vector3 target{};
  math::vector3 velocity{};

  path_corridor corridor{};
  local_boundary boundary{};
}; // struct nav_agent

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_NAV_AGENT_HPP_
