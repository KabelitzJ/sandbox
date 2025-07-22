#ifndef DEMO_CAMERA_CONTROLLER_HPP_
#define DEMO_CAMERA_CONTROLLER_HPP_

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>
#include <libsbx/math/angle.hpp>
#include <libsbx/math/smooth_value.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/devices/input.hpp>

#include <libsbx/scenes/components/camera.hpp>

namespace demo {

class camera_controller {

public:

  using zoom_type = sbx::math::proportional_smooth_value;
  using tilt_type = sbx::math::basic_linear_smooth_value<sbx::math::degree>;

  camera_controller()
  : _orbit_angle{sbx::math::degree{90}}, 
    _tilt_angle{sbx::math::degree{30}},
    _min_tilt_angle{sbx::math::degree{2}},
    _max_tilt_angle{sbx::math::degree{89}},
    _target{sbx::math::vector3{0.0f, 0.0f, 0.0f}},
    _zoom{30.0f},
    _min_zoom{10.0f},
    _max_zoom{200.0f} { }

  auto update() -> void {
    const auto delta_time = sbx::core::engine::delta_time();

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera = scene.camera();

    auto& transform = scene.get_component<sbx::math::transform>(camera);

    // WASD movement

    auto movement = sbx::math::vector3{};

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

    if (sbx::devices::input::is_mouse_button_pressed(sbx::devices::mouse_button::middle)) {
      _last_mouse_position = sbx::devices::input::mouse_position();
    } else if (sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::middle)) {
      auto mouse_position = sbx::devices::input::mouse_position();
      _mouse_position_delta = mouse_position - _last_mouse_position;
      _last_mouse_position = mouse_position;

      _orbit_angle += sbx::math::degree{80.0f * _mouse_position_delta.x() * delta_time.value()};

      const auto offset = sbx::math::degree{80.0f * _mouse_position_delta.y() * delta_time.value()};

      _tilt_angle = tilt_type::clamp(_tilt_angle + offset, _min_tilt_angle.to_degrees(), _max_tilt_angle.to_degrees());
      _tilt_angle.update(delta_time, sbx::math::degree{90});
    } else if (sbx::devices::input::is_mouse_button_released(sbx::devices::mouse_button::middle)) {
      _last_mouse_position = sbx::math::vector2{};
      _mouse_position_delta = sbx::math::vector2{};
    }

    // QE camera rotation

    if (sbx::devices::input::is_key_down(sbx::devices::key::e)) {
      _orbit_angle -= sbx::math::degree{45.0f * delta_time.value()};
    }

    if (sbx::devices::input::is_key_down(sbx::devices::key::q)) {
      _orbit_angle += sbx::math::degree{45.0f * delta_time.value()};
    }

    auto camera_speed = 10.0f;

    if (sbx::devices::input::is_key_down(sbx::devices::key::left_shift)) {
      camera_speed *= 3.0f;
    }

    _target += sbx::math::vector3::normalized(movement) * camera_speed * delta_time.value();

    // Zoom

    auto scroll = sbx::devices::input::scroll_delta();

    static constexpr auto base_scroll_speed = 5.0f;
    static constexpr auto zoom_influence = 0.2f; // smaller = less influence

    const auto offset = base_scroll_speed * std::pow(_zoom, zoom_influence) * scroll.y();

    _zoom = zoom_type::clamp(_zoom - offset, _min_zoom, _max_zoom);
    _zoom.update(delta_time, 10.0f);

    // Calculate camera position

    const auto tilt_angle_rad = sbx::math::to_radians(_tilt_angle.value()).value();

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
  tilt_type _tilt_angle;
  sbx::math::angle _min_tilt_angle;
  sbx::math::angle _max_tilt_angle;
  sbx::math::vector3 _target;
  zoom_type _zoom;
  std::float_t _min_zoom;
  std::float_t _max_zoom;
  sbx::math::vector2 _last_mouse_position;
  sbx::math::vector2 _mouse_position_delta;

}; // class camera_controller

} // namespace demo

#endif // DEMO_CAMERA_CONTROLLER_HPP_
