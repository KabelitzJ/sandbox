#ifndef LIBSBX_PHYSICS_BOX_COLLIDER_HPP_
#define LIBSBX_PHYSICS_BOX_COLLIDER_HPP_

#include <libsbx/math/vector3.hpp>

namespace sbx::physics {

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
