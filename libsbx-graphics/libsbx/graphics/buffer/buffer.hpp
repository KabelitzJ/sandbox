#ifndef LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include <utility>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class buffer : public utility::noncopyable {

public:

  buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, bool map_memory = true);

  buffer(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  buffer(const buffer& source, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  ~buffer();

  auto handle() const noexcept -> const VkBuffer&;

  operator const VkBuffer&() const noexcept;

  auto memory() const noexcept -> const VkDeviceMemory&;

  auto size() const noexcept -> VkDeviceSize;

  auto usage() const noexcept -> VkBufferUsageFlags;

  auto properties() const noexcept -> VkMemoryPropertyFlags;

  auto copy_from(const buffer& source, VkDeviceSize size) const -> void;

  auto copy_from(const buffer& source) const -> void;

  auto write(const void* data, VkDeviceSize size, VkDeviceSize offset = 0) const -> void;

  auto map() -> void;

  auto unmap() -> void;

private:

  VkBuffer _handle{};
  VkDeviceSize _size{};
  VkDeviceMemory _memory{};
  VkBufferUsageFlags _usage{};
  VkMemoryPropertyFlags _properties{};
  void* _mapped{};

}; // class buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
