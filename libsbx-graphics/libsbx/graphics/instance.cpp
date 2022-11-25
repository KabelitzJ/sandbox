#include <libsbx/graphics/instance.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/device_module.hpp>

namespace sbx::graphics {

instance::instance() {
  for (const auto* extension : devices::device_module::get().get_required_extensions()) {
    core::logger::debug("{}", extension);
  }
}

} // namespace sbx::graphics
