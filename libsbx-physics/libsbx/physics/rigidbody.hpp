#ifndef LIBSBX_PHYSICS_RIGIDBODY_HPP_
#define LIBSBX_PHYSICS_RIGIDBODY_HPP_

#include <cmath>

#include <libsbx/math/vector3.hpp>

namespace sbx::physics {

class rigidbody {

public:

  rigidbody(std::float_t mass, bool is_static = false);

  void apply_force(const math::vector3& force);

  void apply_impulse(const math::vector3& impulse);

private:

  std::float_t _mass;

  bool _is_static;

  math::vector3 _velocity;
  math::vector3 _acceleration;

}; // class rigidbody

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_RIGIDBODY_HPP_
