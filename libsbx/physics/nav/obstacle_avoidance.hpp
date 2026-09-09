// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_OBSTACLE_AVOIDANCE_HPP_
#define LIBSBX_PHYSICS_NAV_OBSTACLE_AVOIDANCE_HPP_

#include <span>

#include <libsbx/math/vector3.hpp>

#include <libsbx/physics/nav/local_boundary.hpp>

namespace sbx::physics {

struct avoidance_circle_obstacle {
  math::vector3 position{};
  math::vector3 velocity{};
  std::float_t radius{0.0f};
}; // struct avoidance_circle_obstacle

[[nodiscard]] auto sample_avoidance_velocity(const math::vector3& position, const math::vector3& desired_velocity, std::float_t radius, std::float_t max_speed, std::span<const avoidance_circle_obstacle> neighbors, std::span<const boundary_segment> walls) -> math::vector3;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_OBSTACLE_AVOIDANCE_HPP_
