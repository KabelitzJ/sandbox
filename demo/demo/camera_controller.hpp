#ifndef DEMO_CAMERA_CONTROLLER_HPP_
#define DEMO_CAMERA_CONTROLLER_HPP_

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/transform.hpp>
#include <libsbx/math/angle.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/devices/input.hpp>

#include <libsbx/scenes/components/camera.hpp>

namespace demo {

class camera_controller {

public:

  camera_controller()
  : _orbit_angle{sbx::math::degree{90}},
    _tilt_angle{sbx::math::degree{65}},
    _zoom{30.0f},
    _min_zoom{2.0f},
    _max_zoom{120.0f} { }

  auto update(const sbx::scenes::node player) -> void {
    const auto delta_time = sbx::core::engine::delta_time();

    auto& scenes_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scenes_module.scene();

    auto camera = scene.camera();

    auto& transform = scene.get_component<sbx::math::transform>(camera);

    const auto& player_transform = scene.get_component<sbx::math::transform>(player);

    // Mouse drag camera rotation and movement

    if (sbx::devices::input::is_mouse_button_pressed(sbx::devices::mouse_button::middle)) {
      _last_mouse_position = sbx::devices::input::mouse_position();
    } else if (sbx::devices::input::is_mouse_button_down(sbx::devices::mouse_button::middle)) {
      auto mouse_position = sbx::devices::input::mouse_position();
      _mouse_position_delta = mouse_position - _last_mouse_position;
      _last_mouse_position = mouse_position;

      _orbit_angle += sbx::math::degree{80.0f * _mouse_position_delta.x() * delta_time.value()};
    } else if (sbx::devices::input::is_mouse_button_released(sbx::devices::mouse_button::middle)) {
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

    transform.set_position(sbx::math::vector3{x, height, z});
    transform.look_at(sbx::math::vector3::zero);
  }

private:

  sbx::math::angle _orbit_angle;
  sbx::math::angle _tilt_angle;
  std::float_t _zoom;
  std::float_t _min_zoom;
  std::float_t _max_zoom;
  sbx::math::vector2 _last_mouse_position;
  sbx::math::vector2 _mouse_position_delta;

}; // class camera_controller

} // namespace demo

#endif // DEMO_CAMERA_CONTROLLER_HPP_
