#ifndef LIBSBX_PHYSICS_BOX_COLLIDER_HPP_
#define LIBSBX_PHYSICS_BOX_COLLIDER_HPP_

#include <variant>
#include <algorithm>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/physics/octtree.hpp>

namespace sbx::physics {

struct sphere {
  std::float_t radius;
}; // struct sphere

struct cylinder {
  std::float_t radius;
  std::float_t base;
  std::float_t cap;
}; // struct cylinder

struct capsule {
  std::float_t radius;
  std::float_t base;
  std::float_t cap;
}; // struct capsule

struct box {
  math::vector3 min;
  math::vector3 max;
}; // struct box

using collider = std::variant<sphere, cylinder, capsule, box>;

auto bounding_volume(const collider& collider, const math::vector3& position) -> volume;

struct collider_data {
  const math::vector3& position;
  math::matrix4x4 rotation_scale;
  const collider& collider;
}; // struct collider_data

auto support(const math::vector3& direction, const collider_data& collider) -> math::vector3;

auto gjk(const collider_data& first, const collider_data& second) -> std::optional<math::vector3>;

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_BOX_COLLIDER_HPP_
