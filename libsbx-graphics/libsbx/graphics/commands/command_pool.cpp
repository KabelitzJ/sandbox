#include <libsbx/graphics/commands/command_pool.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

command_pool::command_pool(const std::thread::id& thread_id)
: _thread_id{thread_id} {
  const auto& logical_device = graphics_module::get().logical_device();
  const auto& graphics_queue = logical_device.graphics_queue();

  auto command_pool_create_info = VkCommandPoolCreateInfo{};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = graphics_queue.family();

  validate(vkCreateCommandPool(logical_device, &command_pool_create_info, nullptr, &_handle));
}

command_pool::~command_pool() {
  auto& logical_device = graphics_module::get().logical_device();
  vkDestroyCommandPool(logical_device, _handle, nullptr);
}

auto command_pool::handle() const noexcept -> const VkCommandPool& {
  return _handle;
}

command_pool::operator const VkCommandPool&() const noexcept {
  return _handle;
}

auto command_pool::thread_id() const noexcept -> const std::thread::id& {
  return _thread_id;
}

} // namespace sbx::graphics
