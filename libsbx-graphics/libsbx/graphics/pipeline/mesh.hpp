#ifndef LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_

#include <memory>
#include <vector>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

namespace sbx::graphics {

template<typename Type, VkBufferUsageFlags Flags>
class basic_buffer : public graphics::buffer {

public:

  basic_buffer(const std::vector<Type>& elements, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage = 0u)
  : graphics::buffer{elements.size() * sizeof(Type), (usage | Flags), properties, elements.data()} { }

  basic_buffer(std::size_t size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage)
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

struct submesh {
  std::uint32_t index_count{};
  std::uint32_t index_offset{};
  std::uint32_t vertex_offset{};
}; // struct submesh

template<vertex Vertex>
class mesh {

public:

  using vertex_type = Vertex;
  using vertex_buffer_type = basic_vertex_buffer<Vertex>;

  using index_type = std::uint32_t;
  using index_buffer_type = basic_index_buffer<index_type>;

  mesh(std::vector<vertex_type>&& vertices, std::vector<std::uint32_t>&& indices) {
    auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

    auto& logical_device = graphics_module.logical_device();

    auto fence_create_info = VkFenceCreateInfo{};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    auto fence = VkFence{};

    graphics::validate(vkCreateFence(logical_device, &fence_create_info, nullptr, &fence));

    graphics::validate(vkResetFences(logical_device, 1, &fence));

    auto vertex_buffer_size = sizeof(Vertex) * vertices.size();
    auto index_buffer_size = sizeof(std::uint32_t) * indices.size();

    auto staging_buffer_size = vertex_buffer_size + index_buffer_size;

    auto staging_buffer = graphics::buffer{staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

    staging_buffer.write(vertices.data(), vertex_buffer_size);
    staging_buffer.write(indices.data(), index_buffer_size, vertex_buffer_size);

    _vertex_buffer = std::make_unique<vertex_buffer_type>(vertices.size(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    _index_buffer = std::make_unique<index_buffer_type>(indices.size(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT);

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

    command_buffer.submit(nullptr, nullptr, fence);

    // [TODO] KAJ 2023-03-20 20:07 - This forces the CPU to wait for the GPU to finish copying the data to the device local buffer.
    graphics::validate(vkWaitForFences(logical_device, 1, &fence, true, std::numeric_limits<std::uint64_t>::max()));

    vkDestroyFence(logical_device, fence, nullptr);
  }

  virtual ~mesh() = default;

  auto render(graphics::command_buffer& command_buffer, std::uint32_t instance_count = 1u) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(static_cast<std::uint32_t>(_index_buffer->size()), instance_count, 0, 0, 0);
  }

  auto render_submesh(graphics::command_buffer& command_buffer, const submesh& submesh, std::uint32_t instance_count = 1u) -> void {
    command_buffer.bind_vertex_buffer(0, *_vertex_buffer);
    command_buffer.bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    command_buffer.draw_indexed(submesh.index_count, instance_count, submesh.index_offset, submesh.vertex_offset, 0);
  }

protected:

  mesh()
  : _vertex_buffer{nullptr},
    _index_buffer{nullptr} { }

  std::unique_ptr<vertex_buffer_type> _vertex_buffer{};
  std::unique_ptr<index_buffer_type> _index_buffer{};

}; // class mesh

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
