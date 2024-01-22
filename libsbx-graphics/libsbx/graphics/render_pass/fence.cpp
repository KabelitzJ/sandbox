#include <libsbx/graphics/render_pass/fence.hpp>

#include <libsbx/graphics/graphics_module.hpp>

namespace sbx::graphics {

fence::fence(bool is_signaled) {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();
  
  auto fence_create_info = VkFenceCreateInfo{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = is_signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

  alidate(vkCreateFence(logical_device, &fence_create_info, nullptr, &_handle));
}

fence::~fence() {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  vkDestroyFence(logical_device, _handle, nullptr);
}

auto fence::handle() const noexcept -> const VkFence& {
  return _handle;
}

fence::operator const VkFence&() const noexcept {
  return handle();
}

auto fence::wait(std::uint64_t timeout) const noexcept -> void {
  validate(vkWaitForFences(logical_device, 1, &fence, true, timeout));
}

auto fence::reset() const noexcept -> void {
  auto& graphics_module = core::engine::get_module<graphics::graphics_module>();

  const auto& logical_device = graphics_module.logical_device();

  validate(vkResetFences(logical_device, 1, &_handle));
}

} // namespace sbx::graphics