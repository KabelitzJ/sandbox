// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/obstacle_avoidance.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <vector>

namespace sbx::physics {

inline constexpr auto avoidance_vel_bias = 0.4f;
inline constexpr auto avoidance_weight_desired = 2.0f;
inline constexpr auto avoidance_weight_current = 0.75f;
inline constexpr auto avoidance_weight_side = 0.75f;
inline constexpr auto avoidance_weight_toi = 2.5f;
inline constexpr auto avoidance_horizon = 2.5f;
inline constexpr auto avoidance_adaptive_divs = std::int32_t{7};
inline constexpr auto avoidance_adaptive_rings = std::int32_t{2};
inline constexpr auto avoidance_adaptive_depth = std::int32_t{5};

[[nodiscard]] auto sweep_circle_circle(const math::vector3& c0, std::float_t r0, const math::vector3& v, const math::vector3& c1, std::float_t r1, std::float_t& tmin, std::float_t& tmax) -> bool {
  constexpr auto eps = 0.0001f;

  const auto s = math::vector3{c1.x() - c0.x(), 0.0f, c1.z() - c0.z()};
  const auto r = r0 + r1;
  const auto c = (s.x() * s.x() + s.z() * s.z()) - r * r;

  auto a = v.x() * v.x() + v.z() * v.z();

  if (a < eps) {
    return false;
  }

  const auto b = v.x() * s.x() + v.z() * s.z();
  const auto d = b * b - a * c;

  if (d < 0.0f) {
    return false;
  }

  a = 1.0f / a;

  const auto rd = std::sqrt(d);

  tmin = (b - rd) * a;
  tmax = (b + rd) * a;

  return true;
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

[[nodiscard]] auto closest_point_on_segment_sqr_xz(const math::vector3& point, const math::vector3& a, const math::vector3& b, std::float_t& t) -> std::float_t {
  const auto abx = b.x() - a.x();
  const auto abz = b.z() - a.z();
  const auto apx = point.x() - a.x();
  const auto apz = point.z() - a.z();

  const auto ab_length_squared = abx * abx + abz * abz;

  t = ab_length_squared > 0.0f ? (apx * abx + apz * abz) / ab_length_squared : 0.0f;
  t = std::clamp(t, 0.0f, 1.0f);

  const auto cx = a.x() + abx * t;
  const auto cz = a.z() + abz * t;

  const auto dx = point.x() - cx;
  const auto dz = point.z() - cz;

  return dx * dx + dz * dz;
}

[[nodiscard]] auto time_to_collision_segment(const math::vector3& position, const math::vector3& candidate, const boundary_segment& wall, bool touching) -> std::float_t {
  if (touching) {
    const auto dir_x = wall.end.x() - wall.start.x();
    const auto dir_z = wall.end.z() - wall.start.z();
    const auto normal_x = -dir_z;
    const auto normal_z = dir_x;

    if (normal_x * candidate.x() + normal_z * candidate.z() < 0.0f) {
      return std::numeric_limits<std::float_t>::max();
    }

    return 0.0f;
  }

  auto t = 0.0f;

  if (!intersect_ray_segment(position, candidate, wall.start, wall.end, t)) {
    return std::numeric_limits<std::float_t>::max();
  }

  return t * 2.0f;
}

auto normalize_2d(math::vector3& v) -> void {
  const auto d = std::sqrt(v.x() * v.x() + v.z() * v.z());

  if (d == 0.0f) {
    return;
  }

  const auto inv_d = 1.0f / d;
  v = math::vector3{v.x() * inv_d, 0.0f, v.z() * inv_d};
}

[[nodiscard]] auto rotate_2d(const math::vector3& v, std::float_t angle) -> math::vector3 {
  const auto c = std::cos(angle);
  const auto s = std::sin(angle);

  return math::vector3{v.x() * c - v.z() * s, 0.0f, v.x() * s + v.z() * c};
}

[[nodiscard]] auto build_adaptive_pattern(const math::vector3& desired_velocity) -> std::vector<std::array<std::float_t, 2>> {
  auto pattern = std::vector<std::array<std::float_t, 2>>{};

  constexpr auto nd = avoidance_adaptive_divs;
  constexpr auto nr = avoidance_adaptive_rings;
  const auto da = (1.0f / static_cast<std::float_t>(nd)) * 2.0f * std::numbers::pi_v<std::float_t>;
  const auto ca = std::cos(da);
  const auto sa = std::sin(da);

  auto ddir0 = desired_velocity;
  normalize_2d(ddir0);
  const auto ddir1 = rotate_2d(ddir0, da * 0.5f);

  pattern.push_back(std::array<std::float_t, 2>{0.0f, 0.0f});

  for (auto j = std::int32_t{0}; j < nr; ++j) {
    const auto r = static_cast<std::float_t>(nr - j) / static_cast<std::float_t>(nr);
    const auto& base = (j % 2 == 0) ? ddir0 : ddir1;

    pattern.push_back(std::array<std::float_t, 2>{base.x() * r, base.z() * r});

    auto last1 = pattern.back();
    auto last2 = last1;

    for (auto i = std::int32_t{1}; i < nd - 1; i += 2) {
      const auto next1 = std::array<std::float_t, 2>{last1[0] * ca + last1[1] * sa, -last1[0] * sa + last1[1] * ca};
      const auto next2 = std::array<std::float_t, 2>{last2[0] * ca - last2[1] * sa, last2[0] * sa + last2[1] * ca};

      pattern.push_back(next1);
      pattern.push_back(next2);

      last1 = next1;
      last2 = next2;
    }

    if ((nd & 1) == 0) {
      const auto next2 = std::array<std::float_t, 2>{last2[0] * ca - last2[1] * sa, last2[0] * sa + last2[1] * ca};
      pattern.push_back(next2);
    }
  }

  return pattern;
}

[[nodiscard]] auto process_sample(const math::vector3& candidate, const math::vector3& position, const math::vector3& own_velocity, const math::vector3& desired_velocity, std::float_t radius, std::float_t inv_max_speed, std::span<const avoidance_circle_obstacle> neighbors, std::span<const math::vector3> neighbor_dp, std::span<const math::vector3> neighbor_np, std::span<const boundary_segment> walls, std::span<const std::uint8_t> wall_touching, std::float_t min_penalty) -> std::float_t {
  const auto vpen = avoidance_weight_desired * (math::vector3::distance(candidate, desired_velocity) * inv_max_speed);
  const auto vcpen = avoidance_weight_current * (math::vector3::distance(candidate, own_velocity) * inv_max_speed);

  const auto min_pen = min_penalty - vpen - vcpen;
  const auto threshold_time = (avoidance_weight_toi / min_pen - 0.1f) * avoidance_horizon;

  if (threshold_time - avoidance_horizon > -std::numeric_limits<std::float_t>::epsilon()) {
    return min_penalty;
  }

  auto tmin = avoidance_horizon;
  auto side = 0.0f;
  auto side_count = std::int32_t{0};

  for (auto i = std::size_t{0}; i < neighbors.size(); ++i) {
    const auto vab = candidate * 2.0f - own_velocity - neighbors[i].velocity;

    const auto dot_dp = neighbor_dp[i].x() * vab.x() + neighbor_dp[i].z() * vab.z();
    const auto dot_np = neighbor_np[i].x() * vab.x() + neighbor_np[i].z() * vab.z();

    side += std::clamp(std::min(dot_dp * 0.5f + 0.5f, dot_np * 2.0f), 0.0f, 1.0f);
    ++side_count;

    auto htmin = 0.0f;
    auto htmax = 0.0f;

    if (!sweep_circle_circle(position, radius, vab, neighbors[i].position, neighbors[i].radius, htmin, htmax)) {
      continue;
    }

    if (htmin < 0.0f && htmax > 0.0f) {
      htmin = -htmin * 0.5f;
    }

    if (htmin >= 0.0f && htmin < tmin) {
      tmin = htmin;

      if (tmin < threshold_time) {
        return min_penalty;
      }
    }
  }

  for (auto i = std::size_t{0}; i < walls.size(); ++i) {
    const auto htmin = time_to_collision_segment(position, candidate, walls[i], wall_touching[i] != 0);

    if (htmin < tmin) {
      tmin = htmin;

      if (tmin < threshold_time) {
        return min_penalty;
      }
    }
  }

  const auto side_penalty = (side_count > 0) ? avoidance_weight_side * (side / static_cast<std::float_t>(side_count)) : 0.0f;
  const auto toi_penalty = avoidance_weight_toi * (1.0f / (0.1f + tmin / avoidance_horizon));

  return vpen + vcpen + side_penalty + toi_penalty;
}

[[nodiscard]] auto sample_avoidance_velocity(const math::vector3& position, const math::vector3& own_velocity, const math::vector3& desired_velocity, std::float_t radius, std::float_t max_speed, std::span<const avoidance_circle_obstacle> neighbors, std::span<const boundary_segment> walls) -> math::vector3 {
  const auto inv_max_speed = max_speed > 0.0f ? 1.0f / max_speed : std::numeric_limits<std::float_t>::max();

  auto neighbor_dp = std::vector<math::vector3>{};
  auto neighbor_np = std::vector<math::vector3>{};
  neighbor_dp.reserve(neighbors.size());
  neighbor_np.reserve(neighbors.size());

  for (const auto& neighbor : neighbors) {
    auto dp = math::vector3{neighbor.position.x() - position.x(), 0.0f, neighbor.position.z() - position.z()};
    normalize_2d(dp);

    const auto dv = neighbor.velocity - own_velocity;
    const auto area = dv.x() * dp.z() - dp.x() * dv.z();

    const auto np = (area < 0.01f) ? math::vector3{-dp.z(), 0.0f, dp.x()} : math::vector3{dp.z(), 0.0f, -dp.x()};

    neighbor_dp.push_back(dp);
    neighbor_np.push_back(np);
  }

  auto wall_touching = std::vector<std::uint8_t>{};
  wall_touching.reserve(walls.size());

  for (const auto& wall : walls) {
    auto t = 0.0f;
    wall_touching.push_back(closest_point_on_segment_sqr_xz(position, wall.start, wall.end, t) < 0.0001f ? 1 : 0);
  }

  const auto pattern = build_adaptive_pattern(desired_velocity);

  auto search_radius = max_speed * (1.0f - avoidance_vel_bias);
  auto center = math::vector3{desired_velocity.x() * avoidance_vel_bias, 0.0f, desired_velocity.z() * avoidance_vel_bias};

  for (auto k = std::int32_t{0}; k < avoidance_adaptive_depth; ++k) {
    auto min_penalty = std::numeric_limits<std::float_t>::max();
    auto best_velocity = math::vector3::zero;

    for (const auto& offset : pattern) {
      const auto candidate = math::vector3{center.x() + offset[0] * search_radius, 0.0f, center.z() + offset[1] * search_radius};

      if (candidate.x() * candidate.x() + candidate.z() * candidate.z() > (max_speed + 0.001f) * (max_speed + 0.001f)) {
        continue;
      }

      const auto penalty = process_sample(candidate, position, own_velocity, desired_velocity, radius, inv_max_speed, neighbors, neighbor_dp, neighbor_np, walls, wall_touching, min_penalty);

      if (penalty < min_penalty) {
        min_penalty = penalty;
        best_velocity = candidate;
      }
    }

    center = best_velocity;
    search_radius *= 0.5f;
  }

  return center;
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
