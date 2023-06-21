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

  enum class status : std::uint8_t {
    reset,
    changed,
    normal
  }; // enum class status

  buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, memory::observer_ptr<void> memory = nullptr);

  virtual ~buffer();

  auto handle() const noexcept -> const VkBuffer&;

  operator const VkBuffer&() const noexcept;

  auto memory() const noexcept -> const VkDeviceMemory&;

  auto size() const noexcept -> VkDeviceSize;

  auto map() -> memory::observer_ptr<void>;

  auto unmap() -> void;

  auto write(memory::observer_ptr<void> data, VkDeviceSize size, VkDeviceSize offset = 0) -> void;

protected:

  VkBuffer _handle{};
  VkDeviceSize _size{};
  VkDeviceMemory _memory{};

}; // class buffer

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_BUFFER_BUFFER_HPP_
