#ifndef LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_

#include <memory>
#include <vector>
#include <string>

#include <libsbx/math/volume.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>
#include <libsbx/graphics/buffers/storage_buffer.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/graphics/resource_storage.hpp>

namespace sbx::graphics {

// template<typename Type>
// using vertex_buffer = typed_buffer<Type, (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

// template<typename Type>
// using index_buffer = typed_buffer<Type, (VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

struct submesh {
  std::uint32_t index_count;
  std::uint32_t index_offset;
  std::uint32_t vertex_offset;
  math::volume bounds;
}; // struct submesh

template<vertex Vertex>
class mesh {

public:

  using vertex_type = Vertex;
  // using vertex_buffer_type = vertex_buffer<Vertex>;

  using index_type = std::uint32_t;
  // using index_buffer_type = index_buffer<index_type>;

  struct mesh_data {
    std::vector<vertex_type> vertices;
    std::vector<index_type> indices;
    std::vector<submesh> submeshes;
  }; // struct mesh_data

  mesh(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices, const math::volume& bounds = math::volume{});

  mesh(mesh_data&& mesh_data);

  mesh(const mesh& other) noexcept = delete;

  virtual ~mesh();

  auto render(graphics::command_buffer& command_buffer, std::uint32_t instance_count = 1u) const -> void;

  auto render_submesh(graphics::command_buffer& command_buffer, std::uint32_t submesh_index, std::uint32_t instance_count = 1u) const -> void;

  auto address() const -> std::uint64_t;

  auto bind(graphics::command_buffer& command_buffer) const -> void;

  auto render_submesh_indirect(graphics::storage_buffer& buffer, std::uint32_t offset, std::uint32_t submesh_index, std::uint32_t instance_count = 1u) const -> void;

  auto submeshes() const noexcept -> const std::vector<submesh>&;

  auto submesh_bounds(std::uint32_t submesh_index) const -> math::volume {
    return _submeshes.at(submesh_index).bounds;
  }

protected:

  // mesh()
  // : _vertex_buffer{nullptr},
  //   _index_buffer{nullptr} { }

  auto _upload_vertices(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices) -> void;

  // vertex_buffer_type _vertex_buffer;
  // index_buffer_type _index_buffer;
  buffer_handle _vertex_buffer;
  buffer_handle _index_buffer;
  std::vector<submesh> _submeshes;

}; // class mesh

} // namespace sbx::graphics

#include <libsbx/graphics/pipeline/mesh.ipp>

#endif // LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
