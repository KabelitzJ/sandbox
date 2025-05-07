#ifndef LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_
#define LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_

#include <libsbx/math/matrix4x4.hpp>

namespace sbx::scenes {

struct global_transform {
  math::matrix4x4 matrix;
}; // struct global_transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_
