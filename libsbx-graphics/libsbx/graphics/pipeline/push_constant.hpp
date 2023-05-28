#ifndef LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_

#include <cmath>
#include <cinttypes>

#include <libsbx/math/vector4.hpp>
#include <libsbx/math/color.hpp>

namespace sbx::graphics {

struct push_constant {
  math::color light_color{};
  math::color ambient_color{};
  math::vector4 camera_position{};
  math::vector4 light_position{};
}; // struct push_constant

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
