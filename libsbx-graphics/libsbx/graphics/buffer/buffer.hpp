#ifndef LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_

#include <vulkan/vulkan.hpp>

#include <utility>

#include <libsbx/utility/noncopyable.hpp>

namespace sbx::graphics {

class buffer : public utility::noncopyable {

public:

  buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  buffer(const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  buffer(const buffer& source, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

  ~buffer();

  auto handle() const noexcept -> const VkBuffer&;

  operator const VkBuffer&() const noexcept;

  auto memory() const noexcept -> const VkDeviceMemory&;

  auto size() const noexcept -> VkDeviceSize;

  auto usage() const noexcept -> VkBufferUsageFlags;

  auto properties() const noexcept -> VkMemoryPropertyFlags;

  auto map() const noexcept -> void*;

  auto unmap() const noexcept -> void;

  auto copy_from(const buffer& source, VkDeviceSize size) const noexcept -> void;

  auto copy_from(const buffer& source) const noexcept -> void;

  auto write(const void* data, VkDeviceSize size, VkDeviceSize offset = 0) const noexcept -> void;

private:

  VkBuffer _handle{};
  VkDeviceSize _size{};
  VkDeviceMemory _memory{};
  VkBufferUsageFlags _usage{};
  VkMemoryPropertyFlags _properties{};

}; // class buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
