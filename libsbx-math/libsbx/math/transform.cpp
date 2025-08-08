#include <libsbx/math/transform.hpp>

#include <libsbx/math/matrix_cast.hpp>

namespace sbx::math {

transform::transform(const vector3& position, const quaternion& rotation, const vector3& scale)
: _position{position}, 
  _rotation{rotation}, 
  _scale{scale},
  _rotation_matrix{math::matrix_cast<4, 4>(_rotation)},
  _is_dirty{true} { }

auto transform::position() const noexcept -> const vector3& {
  return _position;
}

auto transform::position() noexcept -> vector3& {
  _is_dirty = true;
  return _position;
}

auto transform::set_position(const vector3& position) noexcept -> void {
  _position = position;
  _is_dirty = true;
}

auto transform::move_by(const vector3& offset) noexcept -> void {
  _position += offset;
  _is_dirty = true;
}

auto transform::rotation() const noexcept -> const quaternion& {
  return _rotation;
}

auto transform::set_rotation(const quaternion& rotation) noexcept -> void {
  _rotation = rotation;
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  _is_dirty = true;
}

auto transform::set_rotation(const vector3& axis, const angle& angle) noexcept -> void {
  _rotation = quaternion{axis, angle};
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  _is_dirty = true;
}

auto transform::scale() noexcept -> vector3& {
  _is_dirty = true;
  return _scale;
}

auto transform::scale() const noexcept -> const vector3& {
  return _scale;
}


auto transform::set_scale(const vector3& scale) noexcept -> void {
  _scale = scale;
  _is_dirty = true;
}

auto transform::forward() const noexcept -> vector3 {
  return -vector3{_rotation_matrix[2]};
}

auto transform::right() const noexcept -> vector3 {
  return vector3{_rotation_matrix[0]};
}

auto transform::up() const noexcept -> vector3 {
  return vector3{_rotation_matrix[1]};
}

auto transform::look_at(const vector3& target) noexcept -> void {
  // [TODO] : Figure out how to directly construct the rotation_matrix
  auto result = matrix4x4::look_at(_position, target, vector3::up);
  _rotation = quaternion{math::matrix4x4::inverted(result)};
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  _is_dirty = true;
}

auto transform::is_dirty() const noexcept -> bool {
  return _is_dirty;
}

auto transform::clear_is_dirty() noexcept -> void {
  _is_dirty = false;
}

} // namespace sbx::math