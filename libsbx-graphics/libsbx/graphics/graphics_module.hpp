#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>

namespace sbx::graphics {

class graphics_module : public core::module<graphics_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<devices::devices_module>{});

public:

  graphics_module();

  ~graphics_module() override;

  auto update([[maybe_unused]] std::float_t delta_time) -> void override;

  static auto validate(VkResult result) -> void;

  instance& instance();

  physical_device& physical_device();

  logical_device& logical_device();
  
private:

  std::unique_ptr<graphics::instance> _instance{};
  std::unique_ptr<graphics::physical_device> _physical_device{};
  std::unique_ptr<graphics::logical_device> _logical_device{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
