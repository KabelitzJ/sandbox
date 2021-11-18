#ifndef SBX_RENDERING_RENDER_BATCH_HPP_
#define SBX_RENDERING_RENDER_BATCH_HPP_

#include <array>

#include <types/gl.hpp>
#include <types/vector.hpp>
#include <types/matrix.hpp>

namespace sbx {

struct vertex {
  vector3 position{};
  vector2 uv{};
  vector3 normal{};
  matrix4x4 model_matrix{};
};

struct render_batch {
  static constexpr auto max_element_count = 5000;

  gl_buffer vertex_array{};
  gl_buffer vertex_buffer{};
  gl_buffer index_buffer{};

  std::array<gl_index, max_element_count> indices{};
  std::size_t index_count{};

  std::array<vertex, max_element_count> vertices{};
  std::size_t vertex_count{};

}; // struct render_batch

} // namespace sbx

#endif // SBX_RENDERING_RENDER_BATCH_HPP_
