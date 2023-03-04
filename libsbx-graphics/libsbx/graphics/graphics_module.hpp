#ifndef LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
#define LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include <libsbx/core/module.hpp>

#include <libsbx/async/async_module.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/utility/hash.hpp>

#include <libsbx/graphics/devices/instance.hpp>
#include <libsbx/graphics/devices/physical_device.hpp>
#include <libsbx/graphics/devices/logical_device.hpp>
#include <libsbx/graphics/devices/surface.hpp>

#include <libsbx/graphics/commands/command_pool.hpp>
#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/render_pass/swapchain.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/shader.hpp>

#include <libsbx/graphics/buffer/buffer.hpp>

#include <libsbx/graphics/mesh.hpp>

namespace sbx::graphics {

/**
 * @brief Checks a @see VkResult and throws an exception if it is an error
 * @param result 
 * @throws @see std::runtime_error 
 */
auto validate(VkResult result) -> void;


/**
 * @brief Module for managing rendering specific tasks
 * 
 * @extends @ref sbx::core::module<T>
 */
class graphics_module : public core::module<graphics_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<async::async_module, devices::devices_module>{});

public:

  graphics_module();

  ~graphics_module() override;

  auto initialize() -> void override;

  auto update([[maybe_unused]] std::float_t delta_time) -> void override;

  auto instance() -> instance&;

  auto physical_device() -> physical_device&;

  auto logical_device() -> logical_device&;

  auto surface() -> surface&;

  auto command_pool(VkQueueFlagBits queue_type = VK_QUEUE_GRAPHICS_BIT, const std::thread::id& thread_id = std::this_thread::get_id()) -> const std::shared_ptr<command_pool>&;

  auto render_pass() -> render_pass&;

  auto swapchain() -> swapchain&;

  auto load_pipeline(const std::filesystem::path& path) -> pipeline&;

  auto pipeline(const std::string& key) -> pipeline&;
  
private:

  auto _start_render_pass() -> void;

  auto _end_render_pass() -> void;

  auto _recreate_swapchain() -> void;

  auto _recreate_command_buffers() -> void;

  struct per_frame_data {
    VkSemaphore image_available_semaphore{};
    VkSemaphore render_finished_semaphore{};
    VkFence in_flight_fence{};
  }; // struct per_frame_data

  struct command_pool_key {
    VkQueueFlagBits queue_type;
    std::thread::id thread_id;

    auto operator==(const command_pool_key& other) const noexcept -> bool {
      return queue_type == other.queue_type && thread_id == other.thread_id;
    }
  }; // struct command_pool_key

  struct command_pool_key_hash {
    auto operator()(const command_pool_key& key) const noexcept -> std::size_t {
      auto hast = std::size_t{0};
      utility::hash_combine(hast, key.queue_type, key.thread_id);
      return hast;
    }
  }; // struct command_pool_key_hash

  std::unique_ptr<graphics::instance> _instance{};
  std::unique_ptr<graphics::physical_device> _physical_device{};
  std::unique_ptr<graphics::logical_device> _logical_device{};

  std::unordered_map<command_pool_key, std::shared_ptr<graphics::command_pool>, command_pool_key_hash> _command_pools{};

  std::unordered_map<std::string, std::unique_ptr<graphics::pipeline>> _pipelines{};

  std::unique_ptr<graphics::surface> _surface{};

  std::unique_ptr<graphics::render_pass> _render_pass{};

  std::unique_ptr<graphics::swapchain> _swapchain{};

  std::vector<per_frame_data> _per_frame_data{};
  std::vector<std::unique_ptr<graphics::command_buffer>> _command_buffers{};

  std::unique_ptr<graphics::mesh> _mesh{};

  std::uint32_t _current_frame{};
  bool _framebuffer_resized{};

}; // class graphics_module

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_GRAPHICS_MODULE_HPP_
