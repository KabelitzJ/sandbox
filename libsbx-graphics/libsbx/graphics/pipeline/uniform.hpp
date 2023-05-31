#ifndef LIBSBX_GRAPHICS_PIPELINE_UNIFORM_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_UNIFORM_HPP_

#include <cmath>

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix4x4.hpp>

namespace sbx::graphics {

struct uniform {
  math::matrix4x4 model{};
  math::matrix4x4 inverse_model{};
  math::matrix4x4 view{};
  math::matrix4x4 projection{};
}; // struct uniform

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_UNIFORM_HPP_
