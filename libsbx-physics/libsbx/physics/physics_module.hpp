#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <cmath>
#include <optional>
#include <vector>

#include <libsbx/core/engine.hpp>
#include <libsbx/core/module.hpp>

#include <libsbx/containers/octree.hpp>

#include <libsbx/math/constants.hpp>
#include <libsbx/math/transform.hpp>
#include <libsbx/math/volume.hpp>

#include <libsbx/physics/collider.hpp>
#include <libsbx/physics/rigidbody.hpp>

#include <libsbx/scenes/components/global_transform.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/utility/logger.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() = default;

  ~physics_module() override = default;

  auto update() -> void override {
    SBX_SCOPED_TIMER("physics_module");

    integrate_rigidbodies();

    // 1. Broad Phase: Use a spatial partition to find potential collision pairs
    const auto potential_pairs = broad_phase();

    // 2. Narrow Phase: Use GJK to confirm collisions and get manifolds
    const auto collistions = narrow_phase(potential_pairs);

    // 3. Resolution: Apply impulses to resolve collisions
    for (const auto& collistion : collistions) {
      resolve_collision(collistion);
    }
  }

private:

  // --- Physics Constants ---
  static constexpr auto linear_damping = 0.90f;
  static constexpr auto angular_damping = 0.88f;
  static constexpr auto restitution = 0.1f;
  static constexpr auto friction_coefficient = 0.6f;
  static constexpr auto max_angular_velocity = 30.0f;
  static constexpr auto baumgarte_bias_factor = 0.005f;
  static constexpr auto penetration_slop = 0.01f; // e.g., 10mm. Adjust as needed.
  
  static constexpr auto motion_epsilon = 1e-3f;
  static constexpr auto penetration_epsilon = 1e-4f;
  static constexpr auto impulse_epsilon = 1e-2f;

  // --- Rigidbody Integration ---

  static auto get_translation(const math::matrix4x4& matrix) -> math::vector3 {
    return { matrix[3].x(), matrix[3].y(), matrix[3].z() };
  }

  static auto get_rotation(const math::matrix4x4& matrix) -> math::quaternion {
    const math::vector3 col0 = { matrix[0].x(), matrix[0].y(), matrix[0].z() };
    const math::vector3 col1 = { matrix[1].x(), matrix[1].y(), matrix[1].z() };
    const math::vector3 col2 = { matrix[2].x(), matrix[2].y(), matrix[2].z() };

    const auto rotation_matrix = math::matrix3x3{math::vector3::normalized(col0), math::vector3::normalized(col1), math::vector3::normalized(col2)};

    return math::quaternion{rotation_matrix};
  }

  static auto from_angular_velocity(const math::vector3& angular_velocity, const units::second delta_time) -> math::quaternion {
    const auto angle = angular_velocity.length() * delta_time;

    if (angle < 1e-6f) {
      return math::quaternion::identity;
    }

    const auto axis = math::vector3::normalized(angular_velocity);

    return math::quaternion{axis, math::angle{math::radian{angle}}};
  }

  auto integrate_rigidbodies() -> void {
    SBX_SCOPED_TIMER("physics_module::integrate_rigidbodies");

    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto query = scene.query<math::transform, const scenes::global_transform, physics::rigidbody>();

    const auto delta_time = core::engine::fixed_delta_time();

    for (auto&& [node, transform, global_transform, rigidbody] : query.each()) {
      if (rigidbody.is_static()) {
        continue;
      }

      const auto world_position = get_translation(global_transform.model);
      const auto world_rotation = get_rotation(global_transform.model);
      const auto rotation_matrix = math::matrix_cast<3, 3>(world_rotation);

      rigidbody.update_inertia_tensor_world(rotation_matrix);

      // Linear motion
      const auto inverse_mass = rigidbody.inverse_mass();
      const auto total_force = rigidbody.constant_forces() + rigidbody.dynamic_forces();
      const auto acceleration = total_force * inverse_mass;

      rigidbody.add_velocity(acceleration * delta_time);
      transform.move_by(rigidbody.velocity() * delta_time);

      // Angular motion
      const auto angular_acceleration = rigidbody.inverse_inertia_tensor_world() * rigidbody.torque();
      rigidbody.add_angular_velocity(angular_acceleration * delta_time);

      const auto delta_rotation = from_angular_velocity(rigidbody.angular_velocity(), delta_time);
      transform.set_rotation(math::quaternion::normalized(delta_rotation * transform.rotation()));

      rigidbody.clear_dynamic_forces();
      rigidbody.clear_torque();
    }
  }

  // --- Collision Pipeline ---
  
  struct collision_pair {
    scenes::node first;
    scenes::node second;
  }; // struct collision_pair

  auto broad_phase() -> std::vector<collision_pair> {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto tree = containers::octree<math::uuid, 16u, 8u>{math::volume{math::vector3{-100.0f}, math::vector3{100.0f}}};
    auto query = scene.query<const physics::collider, const math::transform, const scenes::global_transform, const scenes::id>();

    for (auto&& [node, collider, transform, global_transform, id] : query.each()) {
      const auto volume = bounding_volume(collider, get_translation(global_transform.model));

      if (std::holds_alternative<physics::box>(collider)) {
        const auto& box = std::get<physics::box>(collider);
        scenes_module.add_debug_box(global_transform.model, math::volume{-box.half_extents, box.half_extents}, math::color::red());
      } else if (std::holds_alternative<physics::sphere>(collider)) {
        const auto& sphere = std::get<physics::sphere>(collider);
        scenes_module.add_debug_sphere(get_translation(global_transform.model), sphere.radius, math::color::red());
      }

      tree.insert(id, volume);
    }

    auto pairs = std::vector<collision_pair>{};

    for (const auto& [first_id, second_id] : tree.intersections()) {
      pairs.emplace_back(scene.find_node(first_id), scene.find_node(second_id));
    }
    
    return pairs;
  }

  struct collistion {
    collision_pair nodes;
    collision_manifold manifold;
  }; // struct collistion

  auto narrow_phase(const std::vector<collision_pair>& pairs) -> std::vector<collistion> {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    auto collistions = std::vector<collistion>{};

    for (const auto& pair : pairs) {
      if (pair.first == scenes::node::null || pair.second == scenes::node::null) {
        continue;
      }

      auto& rb1 = scene.get_component<physics::rigidbody>(pair.first);
      auto& rb2 = scene.get_component<physics::rigidbody>(pair.second);

      // Skip static-static collisions
      if (rb1.is_static() && rb2.is_static()) {
        continue;
      }

      const auto& t1 = scene.get_component<math::transform>(pair.first);
      const auto& c1 = scene.get_component<physics::collider>(pair.first);
      const auto rs1 = math::matrix_cast<4, 4>(t1.rotation()) * math::matrix4x4::scaled(math::matrix4x4::identity, t1.scale());
      const auto d1 = collider_data{t1.position(), rs1, c1};
      
      const auto& t2 = scene.get_component<math::transform>(pair.second);
      const auto& c2 = scene.get_component<physics::collider>(pair.second);
      const auto rs2 = math::matrix_cast<4, 4>(t2.rotation()) * math::matrix4x4::scaled(math::matrix4x4::identity, t2.scale());
      const auto d2 = collider_data{t2.position(), rs2, c2};

      // [TODO] Update your GJK function to return a collision_manifold
      if (auto manifold = gjk(d1, d2); manifold) {
        collistions.push_back(collistion{pair, *manifold});
      }
    }
    
    return collistions;
  }

  auto resolve_collision(const collistion& collistion) -> void {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();
    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::fixed_delta_time();

    const auto& nodes = collistion.nodes;
    const auto& manifold = collistion.manifold;

    if (manifold.contact_points.empty()) {
      return;
    }

    auto& t1 = scene.get_component<math::transform>(nodes.first);
    auto& rb1 = scene.get_component<physics::rigidbody>(nodes.first);
    
    auto& t2 = scene.get_component<math::transform>(nodes.second);
    auto& rb2 = scene.get_component<physics::rigidbody>(nodes.second);

    // --- 1. Resolve Penetration ---
    // This creates a small "dead zone" where we don't apply a push-out force.
    const float resolution_depth = std::max(0.0f, manifold.depth - penetration_slop);

    const float total_inverse_mass = rb1.inverse_mass() + rb2.inverse_mass();

    if (total_inverse_mass > 0.0f) {
      const auto avg_contact = std::accumulate(manifold.contact_points.begin(), manifold.contact_points.end(), math::vector3::zero) / static_cast<std::float_t>(manifold.contact_points.size());

      // Compute vectors from centers to contact
      const auto r1 = avg_contact - t1.position();
      const auto r2 = avg_contact - t2.position();

      // Calculate resolution vector
      const auto resolution = manifold.normal * (manifold.depth / total_inverse_mass);

      // Apply positional correction
      t1.move_by(-resolution * rb1.inverse_mass());
      t2.move_by(resolution * rb2.inverse_mass());
    }

    // --- 2. Resolve Velocity (Impulse) ---
    for (const auto& contact_point : manifold.contact_points) {
      const auto r1 = contact_point - t1.position();
      const auto r2 = contact_point - t2.position();

      const auto v1 = rb1.velocity() + math::vector3::cross(rb1.angular_velocity(), r1);
      const auto v2 = rb2.velocity() + math::vector3::cross(rb2.angular_velocity(), r2);
      
      const auto relative_velocity = v2 - v1;
      const auto velocity_along_normal = math::vector3::dot(relative_velocity, manifold.normal);

      // Do not resolve if objects are already moving apart
      if (velocity_along_normal > 0.0f) {
        continue;
      }

      const auto r1_cross_n = math::vector3::cross(r1, manifold.normal);
      const auto r2_cross_n = math::vector3::cross(r2, manifold.normal);

      const auto inverse_mass_sum = rb1.inverse_mass() + rb2.inverse_mass() + math::vector3::dot(r1_cross_n, rb1.inverse_inertia_tensor_world() * r1_cross_n) + math::vector3::dot(r2_cross_n, rb2.inverse_inertia_tensor_world() * r2_cross_n);
      
      if (inverse_mass_sum < math::epsilonf) {
        continue;
      }

      // --- Normal Impulse ---
      const auto penetration_bias = std::max(manifold.depth - penetration_slop, 0.0f);
      // const auto baumgarte_bias = -std::min(0.0f, velocity_along_normal + baumgarte_bias_factor * std::max(manifold.depth - penetration_slop, 0.0f) / delta_time);
      const auto baumgarte_bias = baumgarte_bias_factor * penetration_bias / delta_time;

      auto j = (-(1.0f + restitution) * velocity_along_normal) - baumgarte_bias;

      j = std::max(j, 0.0f);
      j /= inverse_mass_sum;
      j /= static_cast<std::float_t>(manifold.contact_points.size()); // Distribute impulse over contact points

      if (std::abs(j) < 1e-6f) {
        continue; // Ignore tiny normal impulses
      }

      const auto impulse = manifold.normal * j;

      apply_impulse(rb1, -impulse, r1);
      apply_impulse(rb2, impulse, r2);

      // --- Friction Impulse ---
      const auto tangent = math::vector3::normalized(relative_velocity - manifold.normal * velocity_along_normal);
      auto jt = -math::vector3::dot(relative_velocity, tangent);

      jt /= inverse_mass_sum;
      jt /= static_cast<std::float_t>(manifold.contact_points.size());

      const auto friction_impulse = tangent * std::clamp(jt, -j * friction_coefficient, j * friction_coefficient);

      apply_impulse(rb1, -friction_impulse, r1);
      apply_impulse(rb2, friction_impulse, r2);
    }
  }

  auto apply_impulse(rigidbody& body, const math::vector3& impulse, const math::vector3& contact_vector) -> void {
    if (body.is_static()) {
      return;
    }
    
    body.add_velocity(impulse * body.inverse_mass());
    body.add_angular_velocity(body.inverse_inertia_tensor_world() * math::vector3::cross(contact_vector, impulse));
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_