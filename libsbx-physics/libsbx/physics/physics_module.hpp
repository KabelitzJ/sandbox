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
#include <libsbx/physics/box_collider.hpp>
#include <libsbx/physics/quadtree.hpp>
#include <libsbx/physics/octtree.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() {

  }

  ~physics_module() override = default;

  auto update() -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto tree = octree<math::uuid, 16u, 8u>{physics::volume{math::vector3{-100.0f, -100.0f, -100.0f}, math::vector3{100.0f, 100.0f, 100.0f}}};

    auto collider_nodes = scene.query<physics::box_collider>();

    for (auto& node : collider_nodes) {
      auto& transform = node.get_component<math::transform>();
      auto& box_collider = node.get_component<physics::box_collider>();

      const auto& id = node.get_component<scenes::id>();

      const auto& position = transform.position();
      const auto& size = box_collider.size();
      const auto half_size = size / 2.0f;

      const auto min = math::vector3{position.x() - half_size.x(), position.y() - half_size.y(), position.z() - half_size.z()};
      const auto max = math::vector3{position.x() + half_size.x(), position.y() + half_size.y(), position.z() + half_size.z()};

      tree.insert(id, physics::volume{min, max});
    }

    auto intersections = tree.intersections();

    for (const auto& intersection : intersections) {
      auto first_node = scene.find_node(intersection.first);
      auto second_node = scene.find_node(intersection.second);

      if (!first_node || !second_node) {
        continue;
      }

      core::logger::debug("Intersection: {} and {}", first_node->get_component<scenes::tag>(), second_node->get_component<scenes::tag>());
    }

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

private:

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
