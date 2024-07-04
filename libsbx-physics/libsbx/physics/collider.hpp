#ifndef LIBSBX_PHYSICS_BOX_COLLIDER_HPP_
#define LIBSBX_PHYSICS_BOX_COLLIDER_HPP_

#include <variant>
#include <algorithm>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

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

auto support(const math::vector3& direction, const collider& collider, const math::matrix4x4& model, const math::vector3& position) -> math::vector3;

class box_collider {

public:

  box_collider(const math::vector3& size)
  : _size{size} { }

  ~box_collider() = default;

  auto size() const -> const math::vector3& {
    return _size;
  }

  auto set_size(const math::vector3& size) -> void {
    _size = size;
  }

private:

  math::vector3 _size;

}; // class box_collider

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_BOX_COLLIDER_HPP_
