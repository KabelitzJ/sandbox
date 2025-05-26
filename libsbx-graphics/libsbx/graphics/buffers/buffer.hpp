#ifndef LIBSBX_GRAPHICS_BUFFERS_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFERS_BUFFER_HPP_

#include <utility>
#include <span>
#include <cinttypes>

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::graphics {

class buffer : public utility::noncopyable {

public:

  using handle_type = VkBuffer;
  using size_type = VkDeviceSize;

  buffer(size_type size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<const void> memory = nullptr);

  virtual ~buffer();

  auto handle() const noexcept -> handle_type;

  operator handle_type() const noexcept;

  auto address() const noexcept -> std::uint64_t;

  virtual auto size() const noexcept -> size_type;

  virtual auto write(memory::observer_ptr<const void> data, size_type size, size_type offset = 0) -> void;

  // static auto insert_buffer_memory_barrier(command_buffer& command_buffer, buffer&  <-- We maybe need this

protected:

  auto map() -> void;

  auto unmap() -> void;

  memory::observer_ptr<void> _mapped_memory;

private:

  // handle_type _handle{};
  size_type _size{};
  // VkDeviceMemory _memory{};
  VkBufferUsageFlags _usage{};
  VkBuffer _handle;
  VmaAllocation _allocation;
  // VmaAllocationInfo _info;
  std::uint64_t _address;

}; // class buffer

template<typename Type, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties>
class typed_buffer : public buffer {

  using base_type = buffer;

public:

  using value_type = Type;
  using size_type = base_type::size_type;

  typed_buffer(std::span<const Type> elements, VkMemoryPropertyFlags properties = 0, VkBufferUsageFlags usage = 0u)
  : base_type{elements.size() * sizeof(Type), (usage | Usage) , (properties | Properties), elements.data()} { }

  typed_buffer(size_type size, VkMemoryPropertyFlags properties = 0, VkBufferUsageFlags usage = 0u)
  : base_type{size * sizeof(Type), (usage | Usage) , (properties | Properties), nullptr} { }

  ~typed_buffer() override = default;

  auto size() const noexcept -> VkDeviceSize override {
    return size_in_bytes() / sizeof(Type);
  }

  auto size_in_bytes() const noexcept -> VkDeviceSize {
    return base_type::size();
  }

}; // class typed_buffer

using staging_buffer = typed_buffer<std::uint8_t, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)>;

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFERS_BUFFER_HPP_
