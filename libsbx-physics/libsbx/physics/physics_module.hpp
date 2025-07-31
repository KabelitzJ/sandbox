#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>

#include <btBulletDynamicsCommon.h>

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/components/id.hpp>

#include <libsbx/containers/octree.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/physics/rigidbody.hpp>
#include <libsbx/physics/collider.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() {
    _collision_configuration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    _dispatcher = new btCollisionDispatcher{_collision_configuration};

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    _overlapping_pair_cache = new btDbvtBroadphase{};

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    _solver = new btSequentialImpulseConstraintSolver{};

    _dynamics_world = new btDiscreteDynamicsWorld{_dispatcher, _overlapping_pair_cache, _solver, _collision_configuration};

    set_gravity(math::vector3{0.0f, -10.0f, 0.0f});
  }

  ~physics_module() override {
    delete _dynamics_world;

    delete _solver;

    delete _overlapping_pair_cache;

    delete _dispatcher;

    delete _collision_configuration;
  }

  auto set_gravity(const math::vector3& gravity) -> void {
    _dynamics_world->setGravity(_convert_vec3(gravity));
  }

  auto update() -> void override {
    SBX_SCOPED_TIMER("physics_module");

    update_rigidbodies();
    solve_collisions();
  }

  auto _convert_vec3(const math::vector3& vector) -> btVector3 {
    return btVector3{vector.x(), vector.y(), vector.z()};
  }

  btDiscreteDynamicsWorld* _dynamics_world;
  btDefaultCollisionConfiguration* _collision_configuration;
  btCollisionDispatcher* _dispatcher;
  btBroadphaseInterface*  _overlapping_pair_cache;
  btSequentialImpulseConstraintSolver* _solver;

private:

  auto update_rigidbodies() -> void {
    SBX_SCOPED_TIMER("physics_module::update_rigidbodies");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto rigidbody_query = scene.query<physics::rigidbody>();

    for (auto&& [node, rigidbody]  : rigidbody_query.each()) {
      if (rigidbody.is_static()) {
        continue;
      }

      auto& transform = scene.get_component<math::transform>(node);

      const auto total_acceleration = rigidbody.acceleration() + (rigidbody.forces() / rigidbody.mass());

      rigidbody.add_velocity(total_acceleration * delta_time);

      transform.move_by(rigidbody.velocity() * delta_time * 0.95);

      rigidbody.reset_forces();
    }
  }

  auto solve_collisions() -> void {
    SBX_SCOPED_TIMER("physics_module::solve_collisions");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto tree = containers::octree<math::uuid, 16u, 8u>{math::volume{math::vector3{-100.0f, -100.0f, -100.0f}, math::vector3{100.0f, 100.0f, 100.0f}}};

    auto collider_query = scene.query<physics::collider>();

    for (const auto node : collider_query) {
      auto& transform = scene.get_component<math::transform>(node);
      auto& collider = scene.get_component<physics::collider>(node);

      const auto& id = scene.get_component<scenes::id>(node);

      const auto& position = transform.position();

      tree.insert(id, bounding_volume(collider, position));
    }

    auto intersections = tree.intersections();

    for (const auto& intersection : intersections) {
      auto first_node = scene.find_node(intersection.first);
      auto second_node = scene.find_node(intersection.second);

      if (first_node == scenes::node::null || second_node == scenes::node::null) {
        continue;
      }

      auto& first_transform = scene.get_component<math::transform>(first_node);
      const auto& first_collider = scene.get_component<physics::collider>(first_node);

      auto first_rotation_scale = first_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, first_transform.scale());
      const auto first_data = physics::collider_data{first_transform.position(), std::move(first_rotation_scale), first_collider}; 

      auto& second_transform = scene.get_component<math::transform>(second_node);
      const auto& second_collider = scene.get_component<physics::collider>(second_node);

      auto second_rotation_scale = second_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, second_transform.scale());
      const auto second_data = physics::collider_data{second_transform.position(), std::move(second_rotation_scale), second_collider};

      if (auto result = gjk(first_data, second_data); result) {
        auto depth = result->length();
        auto direction = math::vector3::normalized(*result);

        auto& first_rigidbody = scene.get_component<physics::rigidbody>(first_node);
        auto& second_rigidbody = scene.get_component<physics::rigidbody>(second_node);

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
