#include <libsbx/graphics/render_pass/semaphore.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

semaphore::semaphore() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  auto semaphore_create_info = VkSemaphoreCreateInfo{};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  validate(vkCreateSemaphore(logical_device, &semaphore_create_info, nullptr, &_handle));
}

semaphore::~semaphore() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  vkDestroySemaphore(logical_device, _handle, nullptr);
}

auto semaphore::handle() const noexcept -> const VkFence& {
  return _handle;
}

semaphore::operator const VkFence&() const noexcept {
  return handle();
}

} // namespace sbx::graphics
