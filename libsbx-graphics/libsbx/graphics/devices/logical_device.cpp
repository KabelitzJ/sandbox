#include <libsbx/graphics/devices/logical_device.hpp>

namespace sbx::graphics {

logical_device::logical_device(const instance& instance, const physical_device& physical_device) {

}

logical_device::~logical_device() {

}

auto logical_device::handle() const noexcept -> VkDevice {
  return _handle;
}

logical_device::operator VkDevice() const noexcept {
  return _handle;
}

} // namespace sbx::graphics
