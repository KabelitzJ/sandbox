#include <libsbx/physics/rigidbody.hpp>

namespace sbx::physics {

rigidbody::rigidbody(std::float_t mass, std::float_t bounce, bool is_static)
: _mass{mass},
  _bounce{bounce},
  _is_static{is_static} { }

auto rigidbody::mass() const -> std::float_t {
  return _mass;
}

auto rigidbody::set_mass(std::float_t mass) -> void {
  _mass = mass;
}

auto rigidbody::bounce() const -> std::float_t {
  return _bounce;
}

auto rigidbody::set_bounce(std::float_t bounce) -> void {
  _bounce = bounce;
}

auto rigidbody::is_static() const -> bool {
  return _is_static;
}

auto rigidbody::set_is_static(bool is_static) -> void {
  _is_static = is_static;
}

auto rigidbody::apply_force(const math::vector3& force) -> void {
  if (_is_static) {
    return;
  }

  _acceleration += force / _mass;
}

auto rigidbody::velocity() const -> const math::vector3& {
  return _velocity;
}

auto rigidbody::set_velocity(const math::vector3& velocity) -> void {
  _velocity = velocity;
}

auto rigidbody::acceleration() const -> const math::vector3& {
  return _acceleration;
}

auto rigidbody::set_acceleration(const math::vector3& acceleration) -> void {
  _acceleration = acceleration;
}

} // namespace sbx::physics
