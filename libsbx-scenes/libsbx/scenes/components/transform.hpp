#ifndef LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
#define LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_

#include <vector>
#include <memory>
#include <algorithm>

#include <libsbx/memory/observer_ptr.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>

namespace sbx::scenes {

class transform {

public:

  transform()
  : transform{math::vector3{0.0f, 0.0f, 0.0f}, math::vector3{0.0f, 0.0f, 0.0f}, math::vector3{1.0f, 1.0f, 1.0f}} { }

  transform(const math::vector3& position, const math::vector3& rotation, const math::vector3& scale)
  : _position{position},
    _rotation{rotation},
    _scale{scale} { }

  auto local_position() const -> const math::vector3& {
    return _position;
  }

  auto set_local_position(const math::vector3& position) -> void {
    _position = position;
  }

  auto local_rotation() const -> const math::vector3& {
    return _rotation;
  }

  auto set_local_rotation(const math::vector3& rotation) -> void {
    _rotation = rotation;
  }

  auto local_scale() const -> const math::vector3& {
    return _scale;
  }

  auto set_local_scale(const math::vector3& scale) -> void {
    _scale = scale;
  }

private:

  math::vector3 _position;
  math::vector3 _rotation;
  math::vector3 _scale;

}; // class transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_TRANSFORM_HPP_
