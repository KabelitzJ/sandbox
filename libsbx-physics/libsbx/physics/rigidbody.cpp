#include <libsbx/physics/rigidbody.hpp>

namespace sbx::physics {

rigidbody::rigidbody(const units::kilogram& mass, bool is_static)
: _velocity{math::vector3::zero},
  _acceleration{math::vector3::zero},
  _mass{mass},
  _is_static{is_static},
  _forces{math::vector3::zero} { }

auto rigidbody::velocity() const -> const math::vector3& {
  return _velocity;
}

auto rigidbody::set_velocity(const math::vector3& velocity) -> void {
  _velocity = velocity;
}

auto rigidbody::add_velocity(const math::vector3& velocity) -> void {
  _velocity += velocity;
}

auto rigidbody::acceleration() const -> const math::vector3& {
  return _acceleration;
}

auto rigidbody::set_acceleration(const math::vector3& acceleration) -> void {
  _acceleration = acceleration;
}

auto rigidbody::add_acceleration(const math::vector3& acceleration) -> void {
  _acceleration += acceleration;
}

auto rigidbody::mass() const -> const units::kilogram& {
  return _mass;
}

auto rigidbody::set_mass(const units::kilogram& mass) -> void {
  _mass = mass;
}

auto rigidbody::is_static() const -> bool {
  return _is_static;
}

auto rigidbody::apply_force(const math::vector3& force) -> void {
  _forces += force;
}

auto rigidbody::forces() const -> const math::vector3& {
  return _forces;
}

auto rigidbody::reset_forces() -> void {
  _forces = math::vector3::zero;
}

} // namespace sbx::physics
