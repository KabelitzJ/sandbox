#ifndef LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_

#include <cmath>
#include <cinttypes>

#include <libsbx/math/vector4.hpp>

namespace sbx::graphics {

struct push_constant {
  math::vector4 color{};
}; // struct push_constant

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_PUSH_CONSTANT_HPP_
