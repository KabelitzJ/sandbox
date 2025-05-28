#ifndef LIBSBX_SCENES_COMPONENTS_SKYBOX_HPP_
#define LIBSBX_SCENES_COMPONENTS_SKYBOX_HPP_

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/color.hpp>
#include <libsbx/math/vector3.hpp>

#include <libsbx/graphics/images/cube_image.hpp>

namespace sbx::scenes {

struct skybox {
  graphics::cube_image_handle cube_image;
  math::color tint{math::color::white()};
}; // struct skybox

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_SKYBOX_HPP_
