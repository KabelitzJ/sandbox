#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

physical_device::physical_device(const instance& instance) {

}

physical_device::~physical_device() {

}

auto physical_device::handle() const noexcept -> VkPhysicalDevice {
  return _handle;
}

physical_device::operator VkPhysicalDevice() const noexcept {
  return _handle;
}

auto physical_device::_choose_device(const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice {

}

} // namespace sbx::graphics
