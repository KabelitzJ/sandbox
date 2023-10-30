#ifndef LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
#define LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_

#include <libsbx/units/time.hpp>

#include <libsbx/math/transform.hpp>

#include <libsbx/core/module.hpp>
#include <libsbx/core/engine.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/physics/rigidbody.hpp>

namespace sbx::physics {

class physics_module : public core::module<physics_module> {

  inline static const auto is_registered = register_module(stage::pre);

public:

  physics_module() = default;

  ~physics_module() override = default;

  auto update() -> void override {
    auto& scenes_module = core::engine::get_module<scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    const auto delta_time = core::engine::delta_time();

    auto rigidbody_nodes = scene.query<rigidbody>();

    for (auto& rigidbody_node : rigidbody_nodes) {
      auto& rigidbody = rigidbody_node.get_component<physics::rigidbody>();

      if (rigidbody.is_static()) {
        continue;
      }

      auto& transform = rigidbody_node.get_component<math::transform>();

      const auto position = transform.position() + rigidbody.velocity() * delta_time.value();
      transform.set_position(position);

      const auto velocity = rigidbody.velocity() + rigidbody.acceleration() * delta_time.value();
      rigidbody.set_velocity(velocity);
    }
  }

private:

  

}; // class physics_module

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_PHYSICS_MODULE_HPP_
