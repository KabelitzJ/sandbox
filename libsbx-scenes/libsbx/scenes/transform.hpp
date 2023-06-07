#ifndef LIBSBX_SCENES_TRANSFORM_HPP_
#define LIBSBX_SCENES_TRANSFORM_HPP_

#include <libsbx/math/vector3.hpp>

namespace sbx::scenes {

class transform {

public:

  math::vector3 position;
  math::vector3 rotation;
  math::vector3 scale;

  transform() = default;

  transform(const math::vector3& position, const math::vector3& rotation, const math::vector3& scale)
  : position(position), rotation(rotation), scale(scale) {}

  ~transform() = default;

}; // class transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_TRANSFORM_HPP_
