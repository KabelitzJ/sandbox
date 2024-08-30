#ifndef DEMO_CAMERA_CONTROLLER_HPP_
#define DEMO_CAMERA_CONTROLLER_HPP_

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>
#include <libsbx/math/angle.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/scenes/components/camera.hpp>

namespace demo {

class camera_controller {

public:

  camera_controller()
  : _orbit_angle{sbx::math::degree{90}}, 
    _tilt_angle{sbx::math::degree{30}},
    _min_tilt_angle{sbx::math::degree{-89}},
    _max_tilt_angle{sbx::math::degree{89}},
    _target{sbx::math::vector3{0.0f, 2.0f, 0.0f}},
    _zoom{30.0f},
    _min_zoom{2.0f},
    _max_zoom{90.0f} { }

  auto update() -> void {
    const auto delta_time = sbx::core::engine::delta_time();

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera = scene.camera();

    auto& transform = camera.get_component<sbx::math::transform>();

    // WASD movement

    auto movement = sbx::math::vector3{};
    auto is_mouse_movement = false;

    const auto local_forward = sbx::math::vector3::cross(sbx::math::vector3::up, transform.right()).normalize();
    const auto local_right = sbx::math::vector3::cross(sbx::math::vector3::up, transform.forward()).normalize();

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

    // Mouse drag camera rotation and movement

    if (sbx::devices::input::is_mouse_button_pressed(sbx::devices::mouse_button::left) || sbx::devices::input::is_mouse_button_pressed(sbx::devices::mouse_button::right)) {
      _last_mouse_position = sbx::devices::input::mouse_position();
    } else if (sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::left) || sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::right)) {
      auto mouse_position = sbx::devices::input::mouse_position();
      _mouse_position_delta = mouse_position - _last_mouse_position;
      _last_mouse_position = mouse_position;

      if (sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::right)) {
        _orbit_angle += sbx::math::degree{80.0f * _mouse_position_delta.x() * delta_time.value()};
        _tilt_angle = sbx::math::clamp(_tilt_angle + sbx::math::degree{80.0f * _mouse_position_delta.y() * delta_time.value()}, _min_tilt_angle, _max_tilt_angle);
      } 
      
      if (sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::left)) {
        movement += local_forward * _mouse_position_delta.y();
        movement += local_right * _mouse_position_delta.x();
        is_mouse_movement = true;
      }

      // We override 
    } else if (sbx::devices::input::is_mouse_button_released(sbx::devices::mouse_button::left) || sbx::devices::input::is_mouse_button_released(sbx::devices::mouse_button::right)) {
      _last_mouse_position = sbx::math::vector2{};
      _mouse_position_delta = sbx::math::vector2{};
    }

    // QE camera rotation

    if (sbx::devices::input::is_key_down(sbx::devices::key::q)) {
      _orbit_angle -= sbx::math::degree{45.0f * delta_time.value()};
    }

    if (sbx::devices::input::is_key_down(sbx::devices::key::e)) {
      _orbit_angle += sbx::math::degree{45.0f * delta_time.value()};
    }

    auto camera_speed = 10.0f;

    movement.normalize();

    if (is_mouse_movement) {
      movement *= _zoom * 0.3f;
    }

    _target += movement * camera_speed * delta_time.value();

    // Zoom

    auto scroll = sbx::devices::input::scroll_delta();

    _zoom = std::clamp(_zoom - 5.0f * scroll.y(), _min_zoom, _max_zoom);

    // Calculate camera position

    const auto tilt_angle_rad = _tilt_angle.to_radians().value();

    const auto radius = std::cos(tilt_angle_rad) * _zoom;
    const auto height = std::sin(tilt_angle_rad) * _zoom;

    const auto orbit_angle_rad = _orbit_angle.to_radians().value();

    const auto x = std::cos(orbit_angle_rad) * radius;
    const auto z = std::sin(orbit_angle_rad) * radius;

    transform.set_position(_target + sbx::math::vector3{x, height, z});
    transform.look_at(_target);

  }

  auto target() const noexcept -> const sbx::math::vector3& {
    return _target;
  }

private:

  sbx::math::angle _orbit_angle;
  sbx::math::angle _tilt_angle;
  sbx::math::angle _min_tilt_angle;
  sbx::math::angle _max_tilt_angle;
  sbx::math::vector3 _target;
  std::float_t _zoom;
  std::float_t _min_zoom;
  std::float_t _max_zoom;
  sbx::math::vector2 _last_mouse_position;
  sbx::math::vector2 _mouse_position_delta;

}; // class camera_controller

} // namespace demo

#endif // DEMO_CAMERA_CONTROLLER_HPP_
