#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <numbers>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>

namespace sbx::math {

class transform final {

public:

  transform(const vector3& position = vector3::zero, const vector3& euler_angles = vector3::zero, const vector3& scale = vector3::one)
  : _position{position}, 
    _euler_angles{euler_angles}, 
    _scale{scale},
    _rotation_matrix{matrix4x4::rotation_from_euler_angles(_euler_angles)} { }

  ~transform() = default;

  auto position() const noexcept -> const vector3& {
    return _position;
  }

  auto set_position(const vector3& position) noexcept -> void {
    _position = position;
  }

  auto move_by(const vector3& offset) noexcept -> void {
    _position += offset;
  }

  auto euler_angles() const noexcept -> const vector3& {
    return _euler_angles;
  }

  auto set_euler_angles(const vector3& euler_angles) noexcept -> void {
    _euler_angles = euler_angles;
    _rotation_matrix = matrix4x4::rotation_from_euler_angles(_euler_angles);
  }

  auto add_euler_angles(const vector3& offset) noexcept -> void {
    _euler_angles += offset;
    _rotation_matrix = matrix4x4::rotation_from_euler_angles(_euler_angles);
  }

  auto scale() const noexcept -> const vector3& {
    return _scale;
  }

  auto set_scale(const vector3& scale) noexcept -> void {
    _scale = scale;
  }

  auto forward() const noexcept -> vector3 {
    return -math::vector3{_rotation_matrix[2]};
  }

  auto right() const noexcept -> vector3 {
    return math::vector3{_rotation_matrix[0]};
  }

  auto look_at(const vector3& target) noexcept -> void {
    auto direction = vector3::normalized(target - _position);

    const auto pitch = radian{std::asin(-direction.y)};
    const auto yaw = radian{std::atan2(direction.x, direction.z)};
    const auto roll = radian{0.0f};

    _euler_angles = vector3{pitch, yaw, roll};
  }

  auto as_matrix() const -> matrix4x4 {
    const auto translation = matrix4x4::translated(matrix4x4::identity, _position);
    const auto scale = matrix4x4::scaled(matrix4x4::identity, _scale);

    return translation * _rotation_matrix * scale;
  }

private:

  vector3 _position;
  vector3 _euler_angles;
  vector3 _scale;

  math::matrix4x4 _rotation_matrix;

}; // class transform

} // namespace sbx::math

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
