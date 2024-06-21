#ifndef LIBSBX_PHYSICS_RIGIDBODY_HPP_
#define LIBSBX_PHYSICS_RIGIDBODY_HPP_

#include <cmath>

#include <libsbx/units/mass.hpp>

#include <libsbx/math/vector3.hpp>

namespace sbx::physics {

class rigidbody {

public:

  rigidbody(const units::kilogram& mass, bool is_static = false);

  auto velocity() const -> const math::vector3&;

  auto set_velocity(const math::vector3& velocity) -> void;

  auto add_velocity(const math::vector3& velocity) -> void;

  auto acceleration() const -> const math::vector3&;

  auto set_acceleration(const math::vector3& acceleration) -> void;

  auto add_acceleration(const math::vector3& acceleration) -> void;

  auto mass() const -> const units::kilogram&;

  auto set_mass(const units::kilogram& mass) -> void;

  auto is_static() const -> bool;

  auto apply_force(const math::vector3& force) -> void;

  auto forces() const -> const math::vector3&;

  auto reset_forces() -> void;

private:


  math::vector3 _velocity;
  math::vector3 _acceleration;
  units::kilogram _mass;

  bool _is_static;

  math::vector3 _forces;

}; // class rigidbody

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_RIGIDBODY_HPP_
