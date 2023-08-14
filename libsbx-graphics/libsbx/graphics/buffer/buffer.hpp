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

  using size_type = VkDeviceSize;

  buffer(size_type size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<void> memory = nullptr);

  virtual ~buffer();

  auto handle() const noexcept -> const VkBuffer&;

  operator const VkBuffer&() const noexcept;

  auto memory() const noexcept -> const VkDeviceMemory&;

  virtual auto size() const noexcept -> size_type;

  virtual auto write(memory::observer_ptr<const void> data, size_type size, size_type offset = 0) -> void;

protected:

  auto map() -> memory::observer_ptr<void>;

  auto unmap() -> void;

private:

  VkBuffer _handle{};
  VkDeviceSize _size{};
  VkDeviceMemory _memory{};

}; // class buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
