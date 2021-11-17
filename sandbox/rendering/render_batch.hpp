#ifndef SBX_RENDERING_RENDER_BATCH_HPP_
#define SBX_RENDERING_RENDER_BATCH_HPP_

#include <array>

#include <types/primitives.hpp>
#include <types/gl.hpp>

#include "mesh.hpp"

namespace sbx {

/**
 * @brief Represents a batch of meshes that is sent to the gpu to be rendered in a single draw call.
 */
struct render_batch {

  // [TODO] KAJ 2021-11-11 20:54 - Tweak batch size
  static constexpr auto max_element_count = std::size_t{5000u};
  static constexpr auto max_texture_count = std::size_t{16u};

  gl_buffer vertex_array{0};
  gl_buffer vertex_buffer{0};
  gl_buffer index_buffer{0};

  std::array<uint32, max_element_count> indices{};
  std::size_t index_count{0};

  std::array<mesh_vertex, max_element_count> vertices{};
  std::size_t vertex_count{0};

};

} // namespace sbx

#endif // SBX_RENDERING_RENDER_BATCH_HPP_
