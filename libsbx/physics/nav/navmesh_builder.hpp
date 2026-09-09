// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_PHYSICS_NAV_NAVMESH_BUILDER_HPP_
#define LIBSBX_PHYSICS_NAV_NAVMESH_BUILDER_HPP_

#include <libsbx/scenes/scene.hpp>

#include <libsbx/physics/nav/heightfield.hpp>
#include <libsbx/physics/nav/nav_settings.hpp>
#include <libsbx/physics/nav/navmesh.hpp>

namespace sbx::physics {

struct navmesh_build_result {
  navmesh mesh{};
  bool success{false};
}; // struct navmesh_build_result

[[nodiscard]] auto build_navmesh(const config& cfg, scenes::scene& scene) -> navmesh_build_result;

[[nodiscard]] auto build_navmesh(const nav_settings& settings, scenes::scene& scene) -> navmesh_build_result;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_NAV_NAVMESH_BUILDER_HPP_
