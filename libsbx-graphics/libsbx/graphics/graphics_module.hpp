#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include <libsbx/core/module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>
#include <libsbx/graphics/devices/surface.hpp>

#include <libsbx/graphics/commands/command_pool.hpp>
#include <libsbx/graphics/commands/command_buffer.hpp>

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

  surface& surface();

  auto command_pool(const std::thread::id& thread_id = std::this_thread::get_id()) -> const std::shared_ptr<command_pool>&;
  
private:

  std::unique_ptr<graphics::instance> _instance{};
  std::unique_ptr<graphics::physical_device> _physical_device{};
  std::unique_ptr<graphics::logical_device> _logical_device{};

  std::unordered_map<std::thread::id, std::shared_ptr<graphics::command_pool>> _command_pools{};

  std::unique_ptr<graphics::surface> _surface{};
  std::unique_ptr<command_buffer> _command_buffer{};
  VkSemaphore _resent_complete{};
  VkSemaphore _render_complete{};
  VkFence _in_flight_fence{};

  std::size_t _current_frame{};
  bool _framebuffer_resized{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
