#ifndef LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
#define LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_

#include <utility>

#include <cinttypes>
#include <vulkan/vulkan.hpp>

#include <libsbx/utility/noncopyable.hpp>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::graphics {

class buffer : public utility::noncopyable {

public:

  buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<void> memory = nullptr);

  virtual ~buffer();

  auto handle() const noexcept -> const VkBuffer&;

  operator const VkBuffer&() const noexcept;

  auto memory() const noexcept -> const VkDeviceMemory&;

  auto size() const noexcept -> VkDeviceSize;

  virtual auto write(memory::observer_ptr<const void> data, VkDeviceSize size, VkDeviceSize offset = 0) -> void;

protected:

  auto map() -> memory::observer_ptr<void>;

  auto unmap() -> void;

  VkBuffer _handle{};
  VkDeviceSize _size{};
  VkDeviceMemory _memory{};

}; // class buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
