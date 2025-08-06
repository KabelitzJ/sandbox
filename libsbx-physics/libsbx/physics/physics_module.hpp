#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>

#include <btBulletDynamicsCommon.h>

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>
#include <libsbx/math/volume.hpp>
#include <libsbx/math/constants.hpp>

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

  }

  ~physics_module() override {

  }

  auto update() -> void override {
    SBX_SCOPED_TIMER("physics_module");

    update_rigidbodies();
    solve_collisions();
  }

private:

  constexpr static auto motion_epsilon = 0.0001f; // for velocity, angular velocity
  constexpr static auto impulse_epsilon = 0.01f; // for collision impulses
  constexpr static auto penetration_epsilon = 0.0001f; // for GJK penetration resolution

  auto update_rigidbodies() -> void {
    SBX_SCOPED_TIMER("physics_module::update_rigidbodies");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto rigidbody_query = scene.query<physics::rigidbody, math::transform>();

    constexpr float linear_damping = 0.98f;
    constexpr float angular_damping = 0.95f;

    for (auto&& [node, rigidbody, transform] : rigidbody_query.each()) {
      if (rigidbody.is_static() || rigidbody.is_sleeping()) {
        continue;
      }

      // === LINEAR ===

      const auto total_force = rigidbody.constant_forces() + rigidbody.dynamic_forces();
      const auto acceleration = total_force * rigidbody.inverse_mass();

      rigidbody.add_velocity(acceleration * delta_time);
      rigidbody.set_velocity(rigidbody.velocity() * linear_damping);

      transform.move_by(rigidbody.velocity() * delta_time);

      rigidbody.clear_dynamic_forces();

      // === ANGULAR ===

      // 1. Update world-space inverse inertia tensor
      rigidbody.update_inertia_tensor_world(transform.rotation());

      // 2. Convert torque to angular acceleration
      const auto angular_acceleration = rigidbody.inverse_inertia_tensor_world() * rigidbody.torque();

      rigidbody.add_angular_velocity(angular_acceleration * delta_time);
      rigidbody.set_angular_velocity(rigidbody.angular_velocity() * angular_damping);

      constexpr float max_angular_velocity = 10.0f;

      if (rigidbody.angular_velocity().length() > max_angular_velocity) {
        rigidbody.set_angular_velocity(math::vector3::normalized(rigidbody.angular_velocity()) * max_angular_velocity);
      }

      // 3. Integrate rotation using angular velocity
      const auto& w = rigidbody.angular_velocity();
      const auto angle = w.length() * delta_time;

      if (angle > math::epsilonf) {
        const auto axis = math::vector3::normalized(w);
        const auto delta_rotation = math::quaternion{axis, math::angle{math::radian{angle}}};

        transform.set_rotation(math::quaternion::normalized(delta_rotation * transform.rotation()));
      }

      rigidbody.clear_torque();

      // Clamp small motion

      if (rigidbody.velocity().length_squared() < motion_epsilon) {
        rigidbody.set_velocity(math::vector3::zero);
      }

      if (rigidbody.angular_velocity().length_squared() < motion_epsilon) {
        rigidbody.set_angular_velocity(math::vector3::zero);
      }

      // Check for sleeping

      const bool below_linear = rigidbody.velocity().length_squared() < rigidbody::linear_sleep_threshold;
      const bool below_angular = rigidbody.angular_velocity().length_squared() < rigidbody::angular_sleep_threshold;

      if (below_linear && below_angular) {
        if (rigidbody.increment_sleep()) {
          rigidbody.set_velocity(math::vector3::zero);
          rigidbody.set_angular_velocity(math::vector3::zero);
        }
      } else {
        rigidbody.wake();
      }


      utility::logger<"physics">::debug("Sleep counter: {}, Vel: {}, AngVel: {}", rigidbody.sleep_counter(), rigidbody.velocity().length(), rigidbody.angular_velocity().length());
    }
  }

  auto solve_collisions() -> void {
    SBX_SCOPED_TIMER("physics_module::solve_collisions");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    auto tree = containers::octree<math::uuid, 16u, 8u>{math::volume{math::vector3{-100.0f, -100.0f, -100.0f}, math::vector3{100.0f, 100.0f, 100.0f}}};

    auto collider_query = scene.query<const physics::collider, const math::transform, const scenes::id>();

    for (auto&& [node, collider, transform, id] : collider_query.each()) {
      tree.insert(id, bounding_volume(collider, transform.position()));
    }

    auto intersections = tree.intersections();

    for (const auto& [first_id, second_id] : intersections) {
      auto first_node = scene.find_node(first_id);
      auto second_node = scene.find_node(second_id);

      if (first_node == scenes::node::null || second_node == scenes::node::null) {
        continue;
      }

      auto& first_transform = scene.get_component<math::transform>(first_node);
      const auto& first_collider = scene.get_component<physics::collider>(first_node);

      auto& second_transform = scene.get_component<math::transform>(second_node);
      const auto& second_collider = scene.get_component<physics::collider>(second_node);

      const auto first_rotation_scale = first_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, first_transform.scale());
      const auto second_rotation_scale = second_transform.rotation().to_matrix() * math::matrix4x4::scaled(math::matrix4x4::identity, second_transform.scale());

      const auto first_data = collider_data{first_transform.position(), first_rotation_scale, first_collider};
      const auto second_data = collider_data{second_transform.position(), second_rotation_scale, second_collider};

      if (auto result = gjk(first_data, second_data); result) {
        const auto direction = math::vector3::normalized(*result);
        const auto depth = result->length();

        constexpr float max_penetration_resolution = 0.5f;
        const float clamped_depth = std::min(depth, max_penetration_resolution);

        if (clamped_depth < penetration_epsilon) {
          continue;
        }

        utility::logger<"physics">::debug("depth: {}, clamped_depth: {}", depth, clamped_depth);

        auto& rb1 = scene.get_component<physics::rigidbody>(first_node);
        auto& rb2 = scene.get_component<physics::rigidbody>(second_node);

        const auto m1_inv = rb1.inverse_mass();
        const auto m2_inv = rb2.inverse_mass();

        const bool static1 = rb1.is_static();
        const bool static2 = rb2.is_static();

        // Position resolution
        const auto resolution = direction / std::max(1.0f, float(static1 + static2));

        first_transform.move_by(-resolution * clamped_depth * (1 - static1));
        second_transform.move_by(resolution * clamped_depth * (1 - static2));

        const auto v1 = static1 ? math::vector3{} : rb1.velocity();
        const auto v2 = static2 ? math::vector3{} : rb2.velocity();

        const auto relative_velocity = v2 - v1;
        const auto spd = math::vector3::dot(relative_velocity, direction);

        if (spd >= 0.0f) {
          continue;
        }

        // --- Angular impulse logic ---

        const auto contact_point = (first_transform.position() + second_transform.position()) * 0.5f;
        const auto r1 = contact_point - first_transform.position();
        const auto r2 = contact_point - second_transform.position();

        const auto r1xn = math::vector3::cross(r1, direction);
        const auto r2xn = math::vector3::cross(r2, direction);

        const auto I1_inv = rb1.inverse_inertia_tensor_world();
        const auto I2_inv = rb2.inverse_inertia_tensor_world();

        const auto denom = m1_inv + m2_inv + math::vector3::dot(direction, math::vector3::cross(I1_inv * r1xn, r1) + math::vector3::cross(I2_inv * r2xn, r2));

        constexpr auto restitution = 0.00f;

        const auto j = -(1.0f + restitution) * spd / denom;

        constexpr auto max_impulse = 1.0f;
        const float clamped_j = std::clamp(j, -max_impulse, max_impulse);

        if (std::abs(clamped_j) < 0.00001f) {
          continue;
        }

        const auto impulse = direction * clamped_j;

        utility::logger<"physics">::debug("spd: {}, j: {}, clamped_j: {}", spd, j, clamped_j);

        if (!static1) {
          rb1.add_velocity(-impulse * m1_inv);
          rb1.apply_angular_impulse(-impulse, r1);

          if (impulse.length_squared() > impulse_epsilon) {
            rb1.wake();
          }
        }

        if (!static2) {
          rb2.add_velocity(impulse * m2_inv);
          rb2.apply_angular_impulse(impulse, r2);

          if (impulse.length_squared() > impulse_epsilon) {
            rb2.wake();
          }
        }

        // Friction impulse
        auto tangent = relative_velocity - direction * math::vector3::dot(relative_velocity, direction);

        if (tangent.length_squared() > motion_epsilon) {
          tangent = math::vector3::normalized(tangent);

          constexpr float friction = 2.0f;

          const auto vt = math::vector3::dot(relative_velocity, tangent);
          const auto jt = std::clamp(-vt / (m1_inv + m2_inv), -clamped_j * friction, clamped_j * friction);

          if (jt == 0.0f) {
            continue;
          }

          const auto friction_impulse = tangent * jt;

          utility::logger<"physics">::debug("jt: {}", jt);

          if (!static1) {
            rb1.add_velocity(-friction_impulse * m1_inv);
            rb1.apply_angular_impulse(-friction_impulse, r1);
          }
          
          if (!static2) {
            rb2.add_velocity(friction_impulse * m2_inv);
            rb2.apply_angular_impulse(friction_impulse, r2);
          }

          if (friction_impulse.length_squared() > impulse_epsilon) {
            rb1.wake();
            rb2.wake();
          }
        }
      }
    }
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
