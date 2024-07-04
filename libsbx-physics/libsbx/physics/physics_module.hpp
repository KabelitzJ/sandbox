#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>
#include <libsbx/core/logger.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/id.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/physics/rigidbody.hpp>
#include <libsbx/physics/collider.hpp>
#include <libsbx/physics/octtree.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() {

  }

  ~physics_module() override = default;

  auto update() -> void override {
    update_rigidbodies();
    solve_colisions();
  }

private:

  auto update_rigidbodies() -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto rigidbody_nodes = scene.query<rigidbody>();

    for (auto& node : rigidbody_nodes) {
      auto& rigidbody = node.get_component<physics::rigidbody>();

      if (rigidbody.is_static()) {
        continue;
      }

      auto& transform = node.get_component<math::transform>();

      const auto total_acceleration = rigidbody.acceleration() + (rigidbody.forces() / rigidbody.mass());

      rigidbody.add_velocity(total_acceleration * delta_time);

      transform.move_by(rigidbody.velocity() * delta_time);

      rigidbody.reset_forces();
    }
  }

  auto solve_colisions() -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto tree = octree<math::uuid, 16u, 8u>{physics::volume{math::vector3{-100.0f, -100.0f, -100.0f}, math::vector3{100.0f, 100.0f, 100.0f}}};

    auto collider_nodes = scene.query<physics::collider>();

    for (auto& node : collider_nodes) {
      auto& transform = node.get_component<math::transform>();
      auto& collider = node.get_component<physics::collider>();

      const auto& id = node.get_component<scenes::id>();

      const auto& position = transform.position();

      tree.insert(id, bounding_volume(collider, position));
    }

    auto intersections = tree.intersections();

    for (const auto& intersection : intersections) {
      auto first_node = scene.find_node(intersection.first);
      auto second_node = scene.find_node(intersection.second);

      if (!first_node || !second_node) {
        continue;
      }

      auto& first_transform = first_node->get_component<math::transform>();
      const auto& first_collider = first_node->get_component<physics::collider>();

      auto first_rotation_scale = first_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, first_transform.scale());
      const auto first_data = physics::collider_data{first_transform.position(), std::move(first_rotation_scale), first_collider}; 

      auto& second_transform = second_node->get_component<math::transform>();
      const auto& second_collider = second_node->get_component<physics::collider>();

      auto second_rotation_scale = second_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, second_transform.scale());
      const auto second_data = physics::collider_data{second_transform.position(), std::move(second_rotation_scale), second_collider};

      if (auto result = gjk(first_data, second_data); result) {
        core::logger::debug("Collision detected between {} and {}: direction: {}", first_node->get_component<scenes::tag>(), second_node->get_component<scenes::tag>(), *result);
      }
    }
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
