#ifndef LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_

#include <memory>
#include <vector>
#include <string>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

template<typename Type>
using vertex_buffer = basic_buffer<Type, (VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

template<typename Type>
using index_buffer = basic_buffer<Type, (VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

struct submesh {
  std::uint32_t index_count{};
  std::uint32_t index_offset{};
  std::uint32_t vertex_offset{};
}; // struct submesh

template<vertex Vertex>
class mesh {

public:

  using vertex_type = Vertex;
  using vertex_buffer_type = vertex_buffer<Vertex>;

  using index_type = std::uint32_t;
  using index_buffer_type = index_buffer<index_type>;

  mesh(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices) {
    _submeshes.push_back(submesh{static_cast<std::uint32_t>(indices.size()), 0, 0});
    
    _upload_vertices(std::move(vertices), std::move(indices));
  }

  virtual ~mesh() {

  }

  auto render(graphics::command_buffer& command_buffer, std::uint32_t instance_count = 1u) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size()), instance_count, 0, 0, 0);
  }

  auto render_submesh(graphics::command_buffer& command_buffer, std::uint32_t submesh_index, std::uint32_t instance_count = 1u) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    const auto& submesh = _submeshes.at(submesh_index);

    command_buffer.draw_indexed(submesh.index_count, instance_count, submesh.index_offset, 0, 0);
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

protected:

  mesh()
  : _vertex_buffer{nullptr},
    _index_buffer{nullptr} { }

  auto _upload_vertices(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices) -> void {
    auto vertex_buffer_size = sizeof(vertex_type) * vertices.size();
    auto index_buffer_size = sizeof(index_type) * indices.size();

    auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

    // [NOTE] KAJ 2024-01-19 : We basically store two different types in here. The staging_buffer_size is in bytes so we use std::uint8_t.
    auto staging_buffer = graphics::staging_buffer{staging_buffer_size};

    staging_buffer.write(vertices.data(), vertex_buffer_size);
    staging_buffer.write(indices.data(), index_buffer_size, vertex_buffer_size);

    _vertex_buffer = std::make_unique<vertex_buffer_type>(vertices.size());
    _index_buffer = std::make_unique<index_buffer_type>(indices.size());

    auto command_buffer = graphics::command_buffer{true, VK_QUEUE_TRANSFER_BIT};

    {
      auto copy_region = VkBufferCopy{};
      copy_region.size = vertex_buffer_size;
      copy_region.dstOffset = 0;
      copy_region.srcOffset = 0;

      command_buffer.copy_buffer(staging_buffer, *_vertex_buffer, copy_region);
    }

    {
      auto copy_region = VkBufferCopy{};
      copy_region.size = index_buffer_size;
      copy_region.dstOffset = 0;
      copy_region.srcOffset = vertex_buffer_size;

      command_buffer.copy_buffer(staging_buffer, *_index_buffer, copy_region);
    }

    command_buffer.submit_idle();
  }

  std::unique_ptr<vertex_buffer_type> _vertex_buffer{};
  std::unique_ptr<index_buffer_type> _index_buffer{};
  std::vector<submesh> _submeshes{};

}; // class mesh

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
