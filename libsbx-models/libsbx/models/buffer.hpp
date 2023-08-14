#ifndef LIBSBX_MODELS_BUFFER_HPP_
#define LIBSBX_MODELS_BUFFER_HPP_

#include <cinttypes>

#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

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

using index_buffer = basic_index_buffer<std::uint32_t>;

template<typename Type>
using basic_vertex_buffer = basic_buffer<Type, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT>;

using vertex_buffer = basic_vertex_buffer<vertex3d>;

} // namespace sbx::models

#endif // LIBSBX_MODELS_BUFFER_HPP_
