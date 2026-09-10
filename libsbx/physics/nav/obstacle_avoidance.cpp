// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/obstacle_avoidance.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>

namespace sbx::physics {

[[nodiscard]] auto time_to_collision_circle(const math::vector3& position, const math::vector3& velocity, std::float_t radius, const avoidance_circle_obstacle& obstacle) -> std::float_t {
  const auto s = obstacle.position - position;
  const auto v = velocity - obstacle.velocity;

  const auto r = radius + obstacle.radius;
  const auto c = (s.x() * s.x() + s.z() * s.z()) - r * r;
  const auto a = v.x() * v.x() + v.z() * v.z();

  if (a < 0.0001f) {
    return std::numeric_limits<std::float_t>::max();
  }

  const auto b = v.x() * s.x() + v.z() * s.z();
  const auto d = b * b - a * c;

  if (d < 0.0f) {
    return std::numeric_limits<std::float_t>::max();
  }

  const auto inv_a = 1.0f / a;
  const auto rd = std::sqrt(d);
  const auto tmin = (b - rd) * inv_a;

  if (tmin < 0.0f) {
    return (c < 0.0f) ? 0.0f : std::numeric_limits<std::float_t>::max();
  }

  return tmin;
}

[[nodiscard]] auto perp2d(const math::vector3& a, const math::vector3& b) -> std::float_t {
  return a.x() * b.z() - a.z() * b.x();
}

[[nodiscard]] auto intersect_ray_segment(const math::vector3& origin, const math::vector3& direction, const math::vector3& seg_a, const math::vector3& seg_b, std::float_t& t) -> bool {
  const auto v = seg_b - seg_a;
  const auto w = origin - seg_a;

  const auto d = perp2d(direction, v);

  if (std::abs(d) < 1e-6f) {
    return false;
  }

  const auto inv_d = 1.0f / d;
  t = perp2d(v, w) * inv_d;

  if (t < 0.0f || t > 1.0f) {
    return false;
  }

  const auto s = perp2d(direction, w) * inv_d;

  return s >= 0.0f && s <= 1.0f;
}

[[nodiscard]] auto time_to_collision_segment(const math::vector3& position, const math::vector3& velocity, const boundary_segment& wall) -> std::float_t {
  if (velocity.length_squared() < 1e-8f) {
    return std::numeric_limits<std::float_t>::max();
  }

  auto t = 0.0f;

  if (!intersect_ray_segment(position, velocity, wall.start, wall.end, t)) {
    return std::numeric_limits<std::float_t>::max();
  }

  return t;
}

[[nodiscard]] auto sample_avoidance_velocity(const math::vector3& position, const math::vector3& desired_velocity, std::float_t radius, std::float_t max_speed, std::span<const avoidance_circle_obstacle> neighbors, std::span<const boundary_segment> walls) -> math::vector3 {
  constexpr auto horizon = 2.0f;
  constexpr auto weight_desired = 1.0f;
  constexpr auto weight_toi = 3.0f;
  constexpr auto angle_count = std::size_t{16};
  constexpr auto speed_fractions = std::array<std::float_t, 3>{1.0f, 0.75f, 0.5f};

  auto best_velocity = desired_velocity;
  auto best_penalty = std::numeric_limits<std::float_t>::max();

  const auto safe_max_speed = std::max(max_speed, 0.01f);
  const auto desired_speed = desired_velocity.length();
  const auto sample_speed = std::max(desired_speed, max_speed * 0.5f);

  const auto evaluate = [&](const math::vector3& candidate) -> void {
    const auto deviation = math::vector3::distance(candidate, desired_velocity) / safe_max_speed;
    auto penalty = weight_desired * deviation;

    auto min_toi = horizon;

    for (const auto& neighbor : neighbors) {
      min_toi = std::min(min_toi, time_to_collision_circle(position, candidate, radius, neighbor));
    }

    for (const auto& wall : walls) {
      min_toi = std::min(min_toi, time_to_collision_segment(position, candidate, wall));
    }

    penalty += weight_toi * (1.0f / (0.1f + min_toi / horizon));

    if (penalty < best_penalty) {
      best_penalty = penalty;
      best_velocity = candidate;
    }
  };

  evaluate(desired_velocity);

  for (auto a = std::size_t{0}; a < angle_count; ++a) {
    const auto angle = (static_cast<std::float_t>(a) / static_cast<std::float_t>(angle_count)) * 2.0f * std::numbers::pi_v<std::float_t>;
    const auto direction = math::vector3{std::cos(angle), 0.0f, std::sin(angle)};

    for (const auto fraction : speed_fractions) {
      evaluate(direction * (sample_speed * fraction));
    }
  }

  evaluate(math::vector3::zero);

  return best_velocity;
}

[[nodiscard]] auto compute_separation_velocity(const math::vector3& position, std::span<const avoidance_circle_obstacle> neighbors, std::float_t separation_distance, std::float_t weight) -> math::vector3 {
  if (separation_distance < 0.0001f) {
    return math::vector3::zero;
  }

  const auto inv_separation_distance = 1.0f / separation_distance;

  auto displacement = math::vector3::zero;
  auto count = 0.0f;

  for (const auto& neighbor : neighbors) {
    const auto diff = math::vector3{position.x() - neighbor.position.x(), 0.0f, position.z() - neighbor.position.z()};
    const auto distance_squared = diff.length_squared();

    if (distance_squared < 0.00001f || distance_squared > separation_distance * separation_distance) {
      continue;
    }

    const auto distance = std::sqrt(distance_squared);
    const auto normalized_distance = distance * inv_separation_distance;
    const auto push_weight = weight * (1.0f - normalized_distance * normalized_distance);

    displacement = displacement + diff * (push_weight / distance);
    count += 1.0f;
  }

  return (count > 0.0001f) ? displacement * (1.0f / count) : math::vector3::zero;
}

} // namespace sbx::physics
