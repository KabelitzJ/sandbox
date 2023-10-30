#ifndef LIBSBX_PHYSICS_RIGIDBODY_HPP_
#define LIBSBX_PHYSICS_RIGIDBODY_HPP_

#include <cmath>

#include <libsbx/math/vector3.hpp>

namespace sbx::physics {

class rigidbody {

public:

  rigidbody(std::float_t mass, bool is_static = false);

  auto apply_force(const math::vector3& force) -> void;

  auto velocity() const -> const math::vector3&;

  auto set_velocity(const math::vector3& velocity) -> void;

  auto acceleration() const -> const math::vector3&;

  auto set_acceleration(const math::vector3& acceleration) -> void;

  auto mass() const -> std::float_t;

  auto set_mass(std::float_t mass) -> void;

  auto is_static() const -> bool;

  auto set_is_static(bool is_static) -> void;

private:

  std::float_t _mass;

  bool _is_static;

  math::vector3 _velocity;
  math::vector3 _acceleration;

}; // class rigidbody

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_RIGIDBODY_HPP_
