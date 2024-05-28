#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <numbers>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::math {

class transform final {

public:

  transform(const vector3& position = vector3::zero, const quaternion& rotation = quaternion::zero, const vector3& scale = vector3::one)
  : _position{position}, 
    _rotation{rotation}, 
    _scale{scale},
    _rotation_matrix{_rotation.to_matrix()} { }

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

  auto rotation() const noexcept -> const quaternion& {
    return _rotation;
  }

  auto set_rotation(const quaternion& rotation) noexcept -> void {
    _rotation = rotation;
    _rotation_matrix = _rotation.to_matrix();
  }

  auto set_rotation(const vector3& axis, const angle& angle) noexcept -> void {
    _rotation = quaternion{axis, angle};
    _rotation_matrix = _rotation.to_matrix();
  }

  auto scale() const noexcept -> const vector3& {
    return _scale;
  }

  auto set_scale(const vector3& scale) noexcept -> void {
    _scale = scale;
  }

  auto forward() const noexcept -> vector3 {
    return -static_cast<vector3>(_rotation_matrix[2]);
  }

  auto right() const noexcept -> vector3 {
    return math::vector3{_rotation_matrix[0]};
  }

  auto look_at(const vector3& target) noexcept -> void {
    
  }

  auto as_matrix() const -> matrix4x4 {
    const auto translation = matrix4x4::translated(matrix4x4::identity, _position);
    const auto scale = matrix4x4::scaled(matrix4x4::identity, _scale);

    return translation * _rotation_matrix * scale;
  }

private:

  vector3 _position;
  quaternion _rotation;
  vector3 _scale;

  math::matrix4x4 _rotation_matrix;

}; // class transform

} // namespace sbx::math

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
