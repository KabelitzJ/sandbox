#include <libsbx/physics/rigidbody.hpp>

namespace sbx::physics {

rigidbody::rigidbody(const units::kilogram& mass)
: _velocity{math::vector3::zero},
  _mass{mass},
  _constant_forces{math::vector3::zero},
  _dynamic_forces{math::vector3::zero},
  _angular_velocity{math::vector3::zero},
  _torque{math::vector3::zero},
  _inertia{1.0f},
  _inverse_inertia_tensor_local{math::matrix3x3::identity},
  _inverse_inertia_tensor_world{math::matrix3x3::identity},
  _sleep_counter{0u} { }

// === LINEAR ===

auto rigidbody::velocity() const -> const math::vector3& {
  return _velocity;
}

auto rigidbody::set_velocity(const math::vector3& velocity) -> void {
  _velocity = velocity;

  if (_velocity.length_squared() < 0.00001f) {
    _velocity = math::vector3::zero;
  }
}

auto rigidbody::add_velocity(const math::vector3& velocity) -> void {
  _velocity += velocity;

  if (_velocity.length_squared() < 0.00001f) {
    _velocity = math::vector3::zero;
  }
}

auto rigidbody::mass() const -> const units::kilogram& {
  return _mass;
}

auto rigidbody::inverse_mass() const -> std::float_t {
  return is_static() ? 0.0f : 1.0f / std::max(_mass.value(), 0.0001f);
}

auto rigidbody::set_mass(const units::kilogram& mass) -> void {
  _mass = mass;
}

auto rigidbody::is_static() const -> bool {
  return _mass == units::kilogram{0.0f};
}

auto rigidbody::apply_acceleration(const math::vector3& acceleration) -> void {
  _dynamic_forces += acceleration * _mass;
  wake();
}

auto rigidbody::add_constant_acceleration(const math::vector3& acceleration) -> void {
  _constant_forces += acceleration * _mass;
  wake();
}

auto rigidbody::set_constant_acceleration(const math::vector3& acceleration) -> void {
  _constant_forces = acceleration * _mass;
  wake();
}

auto rigidbody::clear_constant_forces() -> void {
  _constant_forces = math::vector3::zero;
}

auto rigidbody::constant_forces() const -> const math::vector3& {
  return _constant_forces;
}

auto rigidbody::dynamic_forces() const -> const math::vector3& {
  return _dynamic_forces;
}

auto rigidbody::clear_dynamic_forces() -> void {
  _dynamic_forces = math::vector3::zero;
}

// === ANGULAR ===

auto rigidbody::angular_velocity() const -> const math::vector3& {
  return _angular_velocity;
}

auto rigidbody::set_angular_velocity(const math::vector3& angular_velocity) -> void {
  _angular_velocity = angular_velocity;

  if (_angular_velocity.length_squared() < 0.00001f) {
    _angular_velocity = math::vector3::zero;
  }
}

auto rigidbody::add_angular_velocity(const math::vector3& angular_velocity) -> void {
  _angular_velocity += angular_velocity;

  if (_angular_velocity.length_squared() < 0.00001f) {
    _angular_velocity = math::vector3::zero;
  }
}

auto rigidbody::apply_torque(const math::vector3& torque) -> void {
  _torque += torque;
  wake();
}

auto rigidbody::clear_torque() -> void {
  _torque = math::vector3::zero;
}

auto rigidbody::torque() const -> const math::vector3& {
  return _torque;
}

auto rigidbody::inertia() const -> std::float_t {
  return _inertia;
}

auto rigidbody::set_inertia(std::float_t inertia) -> void {
  _inertia = inertia;
}

// === INERTIA TENSOR ===

auto rigidbody::set_inverse_inertia_tensor_local(const math::matrix3x3& inverse_tensor) -> void {
  _inverse_inertia_tensor_local = inverse_tensor;
}

auto rigidbody::inverse_inertia_tensor_world() const -> const math::matrix3x3& {
  return _inverse_inertia_tensor_world;
}

auto rigidbody::update_inertia_tensor_world(const math::quaternion& quaternion) -> void {
  const auto rotation = math::matrix_cast(quaternion.to_matrix());

  _inverse_inertia_tensor_world = rotation * _inverse_inertia_tensor_local * math::matrix3x3::transposed(rotation);
}

// === ANGULAR IMPULSE ===

auto rigidbody::apply_angular_impulse(const math::vector3& impulse_world, const math::vector3& contact_vector) -> void {
  const auto angular_impulse = math::vector3::cross(contact_vector, impulse_world);

  _angular_velocity += _inverse_inertia_tensor_world * angular_impulse;
  wake();
}

auto rigidbody::is_sleeping() const noexcept -> bool { 
  return is_static() || _sleep_counter >= sleep_frame_threshold;
}

auto rigidbody::increment_sleep() -> bool {
  _sleep_counter = std::min(_sleep_counter + 1u, sleep_frame_threshold);
  utility::logger<"physics">::debug("Incrementing sleep counter: {}", _sleep_counter);

  return is_sleeping();
}

auto rigidbody::wake() -> void {
  _sleep_counter = 0;
}

} // namespace sbx::physics
