#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/device_module.hpp>

#include <libsbx/graphics/instance.hpp>

namespace sbx::graphics {

class graphics_module : public core::module<graphics_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<devices::device_module>{});

public:

  graphics_module()
  : _instance{std::make_unique<graphics::instance>()} {

  }

  ~graphics_module() override {

  }

  auto update([[maybe_unused]] std::float_t delta_time) -> void override {

  }

  instance& instance() {
    return *_instance;
  }
  
private:

  std::unique_ptr<graphics::instance> _instance{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
