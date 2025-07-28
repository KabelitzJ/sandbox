#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <numbers>

#include <libsbx/ecs/meta.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/angle.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::math {

class transform final {

public:

  transform(const vector3& position = vector3::zero, const quaternion& rotation = quaternion::identity, const vector3& scale = vector3::one)
  : _position{position}, 
    _rotation{rotation}, 
    _scale{scale},
    _rotation_matrix{_rotation.to_matrix()},
    _is_dirty{true} { }

  ~transform() = default;

  auto operator=(const matrix4x4& matrix) -> transform& {
    return *this;
  }

  auto position() const noexcept -> const vector3& {
    return _position;
  }

  auto position() noexcept -> vector3& {
    _is_dirty = true;
    return _position;
  }

  auto set_position(const vector3& position) noexcept -> void {
    _position = position;
    _is_dirty = true;
  }

  auto move_by(const vector3& offset) noexcept -> void {
    _position += offset;
    _is_dirty = true;
  }

  auto rotation() const noexcept -> const quaternion& {
    return _rotation;
  }

  auto set_rotation(const quaternion& rotation) noexcept -> void {
    _rotation = rotation;
    _rotation_matrix = _rotation.to_matrix();
    _is_dirty = true;
  }

  auto set_rotation(const vector3& axis, const angle& angle) noexcept -> void {
    _rotation = quaternion{axis, angle};
    _rotation_matrix = _rotation.to_matrix();
    _is_dirty = true;
  }

  auto scale() noexcept -> vector3& {
    _is_dirty = true;
    return _scale;
  }

  auto scale() const noexcept -> const vector3& {
    return _scale;
  }


  auto set_scale(const vector3& scale) noexcept -> void {
    _scale = scale;
    _is_dirty = true;
  }

  auto forward() const noexcept -> vector3 {
    return -vector3{_rotation_matrix[2]};
  }

  auto right() const noexcept -> vector3 {
    return vector3{_rotation_matrix[0]};
  }

  auto up() const noexcept -> vector3 {
    return vector3{_rotation_matrix[1]};
  }

  auto look_at(const vector3& target) noexcept -> void {
    // [TODO] : Figure out how to directly construct the rotation_matrix
    auto result = matrix4x4::look_at(_position, target, vector3::up);
    _rotation = quaternion{math::matrix4x4::inverted(result)};
    _rotation_matrix = _rotation.to_matrix();
    _is_dirty = true;
  }

  auto as_matrix() const -> matrix4x4 {
    const auto translation = matrix4x4::translated(matrix4x4::identity, _position);
    const auto scale = matrix4x4::scaled(matrix4x4::identity, _scale);

    return translation * _rotation_matrix * scale;
  }

  auto is_dirty() const noexcept -> bool {
    return _is_dirty;
  }

  auto clear_is_dirty() noexcept -> void {
    _is_dirty = false;
  }

private:

  vector3 _position;
  quaternion _rotation;
  vector3 _scale;

  matrix4x4 _rotation_matrix;

  bool _is_dirty;

}; // class transform

} // namespace sbx::math

template<>
struct sbx::ecs::meta<sbx::math::transform> {
  auto operator()(const utility::hashed_string& tag, sbx::math::transform& value) -> void {
    if (tag == "save") {
      
    }
  }
}; // sbx::ecs::meta

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
