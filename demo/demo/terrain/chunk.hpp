#ifndef DEMO_TERRAIN_CHUNK_HPP_
#define DEMO_TERRAIN_CHUNK_HPP_

#include <libsbx/math/uuid.hpp>
#include <libsbx/math/color.hpp>

namespace demo {

struct chunk {
  sbx::math::uuid mesh_id;
  sbx::math::color water_color;
  sbx::math::color land_color;
  sbx::math::color mountain_color;
}; // struct chunk

} // namespace demo

#endif // DEMO_TERRAIN_CHUNK_HPP_
