#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/id.hpp>

#include <libsbx/memory/octtree.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/physics/rigidbody.hpp>
#include <libsbx/physics/collider.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() {

  }

  ~physics_module() override = default;

  auto update() -> void override {
    update_rigidbodies();
    solve_collisions();
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

      transform.move_by(rigidbody.velocity() * delta_time * 0.95);

      rigidbody.reset_forces();
    }
  }

  auto solve_collisions() -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto tree = memory::octree<math::uuid, 16u, 8u>{math::volume{math::vector3{-100.0f, -100.0f, -100.0f}, math::vector3{100.0f, 100.0f, 100.0f}}};

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
        auto depth = result->length();
        auto direction = math::vector3::normalized(*result);

        auto& first_rigidbody = first_node->get_component<physics::rigidbody>();
        auto& second_rigidbody = second_node->get_component<physics::rigidbody>();

        const auto first_is_static = static_cast<std::uint32_t>(first_rigidbody.is_static());
        const auto second_is_static = static_cast<std::uint32_t>(second_rigidbody.is_static());

        auto resolution = direction / std::max(1.0f, static_cast<std::float_t>(first_is_static + second_is_static));

        first_transform.move_by(-resolution * depth * (1u - first_is_static));
        second_transform.move_by(resolution * depth * (1u - second_is_static));

        auto first_velocity = !first_rigidbody.is_static() ? first_rigidbody.velocity() : math::vector3{};
        auto second_velocity = !second_rigidbody.is_static() ? second_rigidbody.velocity() : math::vector3{};

        auto velocity = second_velocity - first_velocity;

        auto spd = math::vector3::dot(velocity, direction);

        auto first_inv_mass = !first_rigidbody.is_static() ? (1.0f / first_rigidbody.mass()) : 1.0f;
        auto second_inv_mass = !second_rigidbody.is_static() ? (1.0f / second_rigidbody.mass()) : 1.0f;

        if (spd >= 0.0f) {
          continue;
        }

        // Restitution * Restitution
        auto j = -(1.0f + (0.5f * 0.5f)) * spd / (first_inv_mass + second_inv_mass);

        auto impulse = direction * j;

        first_rigidbody.add_velocity(-(impulse * first_inv_mass));
        second_rigidbody.add_velocity(impulse * second_inv_mass);
      }
    }
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
