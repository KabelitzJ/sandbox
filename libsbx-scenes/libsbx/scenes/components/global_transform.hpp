#ifndef LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_
#define LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_

#include <libsbx/math/matrix4x4.hpp>

namespace sbx::scenes {

struct global_transform {
  math::matrix4x4 parent{math::matrix4x4::identity};
  math::matrix4x4 model{math::matrix4x4::identity};
  math::matrix4x4 normal{math::matrix4x4::identity};
}; // struct global_transform

} // namespace sbx::scenes

#endif // LIBSBX_SCENE_COMPONENTS_GLOBAL_TRANSFORM_HPP_
