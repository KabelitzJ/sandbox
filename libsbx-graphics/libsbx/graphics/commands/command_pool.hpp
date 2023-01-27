#ifndef LIBSBX_GRAPHICS_COMMANDS_COMMAND_POOL_HPP_
#define LIBSBX_GRAPHICS_COMMANDS_COMMAND_POOL_HPP_

#include <thread>

#include <vulkan/vulkan.hpp>

namespace sbx::graphics {

class command_pool {

public:

  command_pool(const std::thread::id& thread_id = std::this_thread::get_id());

  ~command_pool();

  auto handle() const noexcept -> const VkCommandPool&;

  operator const VkCommandPool&() const noexcept;

  auto thread_id() const noexcept -> const std::thread::id&;

private:

  VkCommandPool _handle{};
  std::thread::id _thread_id{};

}; // class command_pool

} // namespace sbs::graphics

#endif // LIBSBX_GRAPHICS_COMMANDS_COMMAND_POOL_HPP_
