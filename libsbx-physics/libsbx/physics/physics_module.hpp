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
#include <libsbx/math/matrix_cast.hpp>
#include <libsbx/math/angle.hpp>

#include <libsbx/scenes/components/global_transform.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/tag.hpp>
#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/utility/logger.hpp>

#include <libsbx/physics/collider.hpp>
#include <libsbx/physics/rigidbody.hpp>


namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::fixed, dependencies<scenes::scenes_module>{});

public:

  physics_module() = default;

  ~physics_module() override = default;

  auto update() -> void override {
    SBX_SCOPED_TIMER("physics_module");

    integrate_rigidbodies();
  }

private:
  
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

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
