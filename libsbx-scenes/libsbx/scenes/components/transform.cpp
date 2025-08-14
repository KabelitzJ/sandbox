#include <libsbx/scenes/components/transform.hpp>

#include <libsbx/math/matrix_cast.hpp>

namespace sbx::scenes {

transform::transform(const math::vector3& position, const math::quaternion& rotation, const math::vector3& scale)
: _position{position}, 
  _rotation{rotation}, 
  _scale{scale},
  _rotation_matrix{math::matrix_cast<4, 4>(_rotation)},
  _is_dirty{true} { }

auto transform::position() const noexcept -> const math::vector3& {
  return _position;
}

auto transform::position() noexcept -> math::vector3& {
  mart_dirty();
  return _position;
}

auto transform::set_position(const math::vector3& position) noexcept -> void {
  _position = position;
  mart_dirty();
}

auto transform::move_by(const math::vector3& offset) noexcept -> void {
  _position += offset;
  mart_dirty();
}

auto transform::rotation() const noexcept -> const math::quaternion& {
  return _rotation;
}

auto transform::set_rotation(const math::quaternion& rotation) noexcept -> void {
  _rotation = rotation;
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  mart_dirty();
}

auto transform::set_rotation(const math::vector3& axis, const math::angle& angle) noexcept -> void {
  _rotation = math::quaternion{axis, angle};
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  mart_dirty();
}

auto transform::scale() noexcept -> math::vector3& {
  mart_dirty();
  return _scale;
}

auto transform::scale() const noexcept -> const math::vector3& {
  return _scale;
}


auto transform::set_scale(const math::vector3& scale) noexcept -> void {
  _scale = scale;
  mart_dirty();
}

auto transform::forward() const noexcept -> math::vector3 {
  return -math::vector3{_rotation_matrix[2]};
}

auto transform::right() const noexcept -> math::vector3 {
  return math::vector3{_rotation_matrix[0]};
}

auto transform::up() const noexcept -> math::vector3 {
  return math::vector3{_rotation_matrix[1]};
}

auto transform::look_at(const math::vector3& target) noexcept -> void {
  // [TODO] : Figure out how to directly construct the rotation_matrix
  auto result = math::matrix4x4::look_at(_position, target, math::vector3::up);
  _rotation = math::quaternion{math::matrix4x4::inverted(result)};
  _rotation_matrix = math::matrix_cast<4, 4>(_rotation);
  mart_dirty();
}

auto transform::is_dirty() const noexcept -> bool {
  return _is_dirty;
}

auto transform::clear_is_dirty() noexcept -> void {
  _is_dirty = false;
}

} // namespace sbx::scenes