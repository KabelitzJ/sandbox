#ifndef DEMO_PLAYER_CONTROLLER_HPP_
#define DEMO_PLAYER_CONTROLLER_HPP_

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/devices/input.hpp>

#include <libsbx/scenes/node.hpp>
#include <libsbx/scenes/scenes_module.hpp>

namespace demo {

class player_controller {

public:

  player_controller()
  : _speed{5.0f}, 
    _sprint_speed{10.0f} { }

  auto update(const sbx::scenes::node player) -> void {
    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera = scene.camera();

    const auto delta_time = sbx::core::engine::delta_time();
    auto movement = sbx::math::vector3{};

    auto& transform = scene.get_component<sbx::math::transform>(player);

    const auto& camera_transform = scene.get_component<sbx::math::transform>(camera);

    const auto local_forward = sbx::math::vector3::cross(sbx::math::vector3::up, camera_transform.right()).normalize();
    const auto local_right = sbx::math::vector3::cross(sbx::math::vector3::up, camera_transform.forward()).normalize();

    if (sbx::devices::input::is_key_down(sbx::devices::key::w)) {
      movement += local_forward;
    }

    if (sbx::devices::input::is_key_down(sbx::devices::key::s)) {
      movement -= local_forward;
    }

    if (sbx::devices::input::is_key_down(sbx::devices::key::a)) {
      movement += local_right;
    }

    if (sbx::devices::input::is_key_down(sbx::devices::key::d)) {
      movement -= local_right;
    }

    const auto speed = sbx::devices::input::is_key_down(sbx::devices::key::left_shift) ? _sprint_speed : _speed;

    transform.move_by(movement * speed * delta_time.value());
  }

private:

  std::float_t _speed;
  std::float_t _sprint_speed;

}; // class player_controller

} // namespace demo

#endif // DEMO_PLAYER_CONTROLLER_HPP_
