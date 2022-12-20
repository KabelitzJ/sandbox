#include <libsbx/graphics/devices/physical_device.hpp>

namespace sbx::graphics {

physical_device::physical_device() {

}

physical_device::~physical_device() {

}

VkPhysicalDevice physical_device::handle() const noexcept {
  return _handle;
}

physical_device::operator VkPhysicalDevice() const noexcept {
  return _handle;
}

} // namespace sbx::graphics
