#include <libsbx/graphics/instance.hpp>

#include <libsbx/core/logger.hpp>

#include <libsbx/devices/device_module.hpp>

namespace sbx::graphics {

instance::instance() {
  for (const auto* extension : devices::device_module::get().required_extentions()) {
    core::logger::debug("{}", extension);
  }

  _initialize();
}

instance::~instance() {
  _terminate();
}

void instance::_initialize() {

}

void instance::_terminate() {
  
}

} // namespace sbx::graphics
