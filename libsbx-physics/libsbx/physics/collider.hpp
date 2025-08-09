#ifndef LIBSBX_PHYSICS_COLLIDER_HPP_
#define LIBSBX_PHYSICS_COLLIDER_HPP_

#include <variant>

#include <libsbx/units/mass.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix3x3.hpp>

namespace sbx::physics {

struct box {
  math::vector3 half_extents;
}; // struct box

using collider = std::variant<box>;

inline auto compute_inverse_inertia_tensor(const box& box, const units::kilogram& mass) -> math::matrix3x3 {
  const float m = std::max(mass.value(), 0.0001f);
  const float w = box.half_extents.x();
  const float h = box.half_extents.y();
  const float d = box.half_extents.z();

  return math::matrix3x3{
    12.0f / (m * (h * h + d * d)), 0.0f, 0.0f,
    0.0f, 12.0f / (m * (w * w + d * d)), 0.0f,
    0.0f, 0.0f, 12.0f / (m * (w * w + h * h))
  };
}

inline auto compute_inverse_inertia_tensor(const collider& collider, const units::kilogram& mass) -> math::matrix3x3 {
  return std::visit([&](const auto& shape) { return compute_inverse_inertia_tensor(shape, mass); }, collider);
}

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_COLLIDER_HPP_
