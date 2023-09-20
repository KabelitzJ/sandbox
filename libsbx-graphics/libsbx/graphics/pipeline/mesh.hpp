#ifndef LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_

#include <memory>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

template<typename Type, VkBufferUsageFlags Flags>
class basic_buffer : public graphics::buffer {

public:

  basic_buffer(std::size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
  : graphics::buffer{size * sizeof(Type), (usage | Flags) , properties} { }

  ~basic_buffer() override {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    const auto& logical_device = graphics_module.logical_device();
    
    logical_device.wait_idle();
  }

  auto size() const noexcept -> VkDeviceSize override {
    return size_in_bytes() / sizeof(Type);
  }

  auto size_in_bytes() const noexcept -> VkDeviceSize {
    return graphics::buffer::size();
  }

}; // class basic_buffer

template<typename Type>
using basic_index_buffer = basic_buffer<Type, VK_BUFFER_USAGE_INDEX_BUFFER_BIT>;

template<typename Type>
using basic_vertex_buffer = basic_buffer<Type, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT>;

template<vertex Vertex>
class mesh {

public:

  using vertex_type = Vertex;
  using vertex_buffer_type = basic_vertex_buffer<Vertex>;

  using index_type = std::uint32_t;
  using index_buffer_type = basic_index_buffer<index_type>;

  mesh(std::unique_ptr<vertex_buffer_type> vertex_buffer, std::unique_ptr<index_buffer_type> index_buffer)
  : _vertex_buffer{std::move(vertex_buffer)},
    _index_buffer{std::move(index_buffer)} { }

  virtual ~mesh() = default;

  auto render(graphics::command_buffer& command_buffer) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size()), 1, 0, 0, 0);
  }

protected:

  std::unique_ptr<vertex_buffer_type> _vertex_buffer{};
  std::unique_ptr<index_buffer_type> _index_buffer{};

}; // class mesh

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
