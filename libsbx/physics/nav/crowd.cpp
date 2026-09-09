// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#include <libsbx/physics/nav/crowd.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

#include <libsbx/math/quaternion.hpp>

#include <libsbx/scenes/components.hpp>
#include <libsbx/scenes/node.hpp>

#include <libsbx/physics/nav/local_boundary.hpp>
#include <libsbx/physics/nav/navmesh_query.hpp>
#include <libsbx/physics/nav/obstacle_avoidance.hpp>
#include <libsbx/physics/nav/path_corridor.hpp>

namespace sbx::physics {

auto crowd::request_move_target(nav_agent& agent, const navmesh& mesh, const math::vector3& target) -> bool {
  const auto start_ref = find_nearest_poly(mesh, agent.corridor.position);
  const auto end_ref = find_nearest_poly(mesh, target);

  if (start_ref == null_poly_ref || end_ref == null_poly_ref) {
    agent.state = nav_agent_state::target_unreachable;

    return false;
  }

  const auto result = find_path(mesh, start_ref, end_ref, agent.corridor.position, target);

  if (!result.success) {
    agent.state = nav_agent_state::target_unreachable;

    return false;
  }

  corridor_set_corridor(agent.corridor, target, result.polys);

  agent.target = target;
  agent.state = nav_agent_state::moving;

  return true;
}

auto crowd::update(scenes::scene& scene, const navmesh& mesh, std::float_t dt) -> void {
  _grid.clear();

  for (auto&& [entity, agent, transform] : scene.query<nav_agent, scenes::local_transform>().each()) {
    static_cast<void>(agent);

    _grid.insert(scene.node_of(entity), transform.position);
  }

  auto neighbor_nodes = std::vector<scenes::node>{};
  auto neighbor_obstacles = std::vector<avoidance_circle_obstacle>{};

  for (auto&& [entity, agent, transform] : scene.query<nav_agent, scenes::local_transform>().each()) {
    auto node = scene.node_of(entity);

    if (agent.corridor.path.empty()) {
      const auto start_ref = find_nearest_poly(mesh, transform.position);
      corridor_reset(agent.corridor, start_ref, transform.position);
    }

    if (agent.state != nav_agent_state::moving) {
      continue;
    }

    corridor_move_position(agent.corridor, mesh, transform.position);

    const auto corners = corridor_find_corners(agent.corridor, mesh, 4);

    if (corners.empty()) {
      agent.state = nav_agent_state::idle;
      agent.velocity = math::vector3::zero;

      continue;
    }

    const auto& next_corner = corners.front();
    const auto to_corner = next_corner.position - transform.position;
    const auto distance_to_corner = to_corner.length();

    if (corners.size() == 1 && distance_to_corner < 0.2f) {
      agent.state = nav_agent_state::idle;
      agent.velocity = math::vector3::zero;

      continue;
    }

    const auto desired_direction = (distance_to_corner > 0.0001f) ? (to_corner / distance_to_corner) : math::vector3::zero;

    auto desired_speed = agent.max_speed;

    if (corners.size() == 1) {
      desired_speed = std::min(agent.max_speed, distance_to_corner / std::max(dt, 0.0001f));
    }

    const auto desired_velocity = desired_direction * desired_speed;

    auto target_velocity = desired_velocity;

    if (agent.obstacle_avoidance_enabled || agent.separation_enabled) {
      local_boundary_update(agent.boundary, mesh, agent.corridor.path.front(), transform.position, agent.collision_query_range);

      neighbor_nodes.clear();
      _grid.query(transform.position, agent.collision_query_range, neighbor_nodes);

      neighbor_obstacles.clear();

      for (const auto& neighbor_node : neighbor_nodes) {
        if (neighbor_node == node) {
          continue;
        }

        const auto neighbor_agent = neighbor_node.try_get_component<nav_agent>();
        const auto neighbor_transform = neighbor_node.try_get_component<scenes::local_transform>();

        if (!neighbor_agent || !neighbor_transform) {
          continue;
        }

        neighbor_obstacles.push_back(avoidance_circle_obstacle{neighbor_transform->position, neighbor_agent->velocity, neighbor_agent->radius});
      }

      target_velocity = sample_avoidance_velocity(transform.position, desired_velocity, agent.radius, agent.max_speed, neighbor_obstacles, agent.boundary.segments);
    }

    const auto velocity_delta = target_velocity - agent.velocity;
    const auto max_delta = agent.max_acceleration * dt;
    const auto delta_length = velocity_delta.length();

    agent.velocity = (delta_length > max_delta && delta_length > 0.0001f)
      ? agent.velocity + velocity_delta * (max_delta / delta_length)
      : target_velocity;

    transform.position = transform.position + agent.velocity * dt;

    if (agent.velocity.length_squared() > 0.0001f) {
      transform.rotation = math::quaternion::look_at(math::vector3::normalized(agent.velocity));
    }
  }
}

} // namespace sbx::physics
