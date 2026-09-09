// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_CROWD_HPP_
#define LIBSBX_PHYSICS_NAV_CROWD_HPP_

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/scenes/scene.hpp>

#include <libsbx/physics/nav/nav_agent.hpp>
#include <libsbx/physics/nav/navmesh.hpp>
#include <libsbx/physics/nav/proximity_grid.hpp>

namespace sbx::physics {

class crowd final : public utility::noncopyable {

public:

  crowd() = default;

  auto request_move_target(nav_agent& agent, const navmesh& mesh, const math::vector3& target) -> bool;

  auto update(scenes::scene& scene, const navmesh& mesh, std::float_t dt) -> void;

private:

  proximity_grid _grid{};

}; // class crowd

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_CROWD_HPP_
