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

#include <libsbx/models/mesh.hpp>

#include <libsbx/physics/rigidbody.hpp>
#include <libsbx/physics/box_collider.hpp>

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

  struct collision_info {

  }; // struct collision_info

  auto _test_collision(const scenes::node& a, const scenes::node& b) -> std::optional<collision_info> {
    const auto& a_transform = a.get_component<math::transform>();
    const auto& b_transform = b.get_component<math::transform>();

    const auto& a_collider = a.get_component<physics::box_collider>();
    const auto& b_collider = b.get_component<physics::box_collider>();

    const auto a_min = a_transform.position() - a_collider.size() / 2.0f;
    const auto a_max = a_transform.position() + a_collider.size() / 2.0f;

    const auto b_min = b_transform.position() - b_collider.size() / 2.0f;
    const auto b_max = b_transform.position() + b_collider.size() / 2.0f;

    if (a_min.x() > b_max.x() || a_max.x() < b_min.x()) {
      return std::nullopt;
    }

    if (a_min.y() > b_max.y() || a_max.y() < b_min.y()) {
      return std::nullopt;
    }

    if (a_min.z() > b_max.z() || a_max.z() < b_min.z()) {
      return std::nullopt;
    }

    return collision_info{};
  }

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
