#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <string>
#include <memory>
#include <iostream>
#include <chrono>
#include <cstddef>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "configuration.hpp"
#include "event_manager.hpp"
#include "context.hpp"
#include "monitor.hpp"
#include "window.hpp"
#include "input.hpp"

#include "instance.hpp"
#include "physical_device.hpp"
#include "logical_device.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "command_pool.hpp"

#include "time.hpp"
#include "events.hpp"
#include "hashed_string.hpp"
#include "key.hpp"

#include "buffer.hpp"
#include "vertex.hpp"

namespace demo {

class engine {

public:

  /**
   * @brief Construct a new application
   * 
   * @param configuration The path to the configuration file
   */
  engine(const std::filesystem::path& config_path, const std::vector<std::string>& cli_args)
  : _config_path{config_path},
    _cli_args{cli_args},
    _is_running{false},
    _is_paused{false} { }

  ~engine() {
    for (const auto& subscription : _subscriptions) {
      _event_manager->unsubscribe(subscription);
    }
  }

  /**
   * @brief Initializes and runs the engine.
   * 
   * @returns EXIT_SUCCESS if the engine ran successfully, EXIT_FAILURE if an error occurred.
   */
  sbx::int32 start() {
    try {
      _initialize();
    } catch (const std::exception& exception) {
      _logger->error("Exception during initialization: {}", exception.what());
      return EXIT_FAILURE;
    }

    try {
      _run();
    } catch (const std::exception& exception) {
      _logger->error("Exception during execution: {}", exception.what());
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

private:

  /**
   * @brief Initializes all systems of the engine.
   */
  void _initialize() {
    // Set up all systems - ORDER MATTERS (dependencies)

    // Core systems
    _configuration = std::make_unique<configuration>(_config_path);
    _logger = std::make_unique<logger>(_configuration.get());
    _event_manager = std::make_unique<event_manager>(_logger.get());

    // Window related systems
    _context = std::make_unique<context>();
    _monitor = std::make_unique<monitor>(_event_manager.get());
    _window = std::make_unique<window>(_configuration.get(), _event_manager.get(), _monitor.get());
    _input = std::make_unique<input>(_event_manager.get());

    // Vulkan related systems
    _instance = std::make_unique<instance>(_logger.get(), _window.get(), _configuration.get());
    _surface = std::make_unique<surface>(_window.get(), _instance.get());
    _physical_device = std::make_unique<physical_device>(_logger.get(), _instance.get(), _surface.get());
    _logical_device = std::make_unique<logical_device>(_instance.get(), _physical_device.get());
    _command_pool = std::make_unique<command_pool>(_physical_device.get(), _logical_device.get());
    _swapchain = std::make_unique<swapchain>(_window.get(), _surface.get(), _physical_device.get(), _logical_device.get(), _command_pool.get(), _event_manager.get());
    _pipeline = std::make_unique<pipeline>("demo/assets/shaders/basic", _logical_device.get(), _swapchain.get());

    const auto vertices = std::array<vertex, 3>{
      vertex{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      vertex{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    _vertex_buffer = std::make_unique<buffer<vertex, 3>>(_physical_device.get(), _logical_device.get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _vertex_buffer->map(vertices);

    // Event listeners
    _subscriptions.emplace_back(_event_manager->subscribe<window_closed_event>([this](const auto&) {
      _is_running = false;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<window_minimized_event>([this](const auto&) {
      _is_paused = true;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<window_maximized_event>([this](const auto&) {
      _is_paused = false;
    }));

    _subscriptions.emplace_back(_event_manager->subscribe<window_restored_event>([this](const auto&) {
      _is_paused = false;
    }));
  }

  /**
   * @brief Runs the engine.
   */
  void _run() {
    using clock = std::chrono::high_resolution_clock;

    _is_running = true;

    _window->show();

    auto start = clock::now();

    while (_is_running) {
      _input->_update();
      _window->poll_events();

      const auto now = clock::now();
      const auto delta = time{now - start};
      start = now;

      if (_input->is_key_pressed(key::escape)) {
        _is_running = false;
      }

      if (_is_paused) {
        continue;
      }

      _swapchain->prepare_frame();
      _record_command_buffer();
      _swapchain->draw_frame();
    }

    _logical_device->wait_till_idle();
  }

  void _record_command_buffer() {
    const auto command_buffer_begin_info = VkCommandBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr
    };
    
    if (vkBeginCommandBuffer(_swapchain->current_command_buffer(), &command_buffer_begin_info) != VK_SUCCESS) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    const auto clear_color = VkClearValue{
      .color = {
        .float32 = { 0.0f, 0.0f, 0.0f, 1.0f }
      }
    };

    const auto render_pass_info = VkRenderPassBeginInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = _swapchain->render_pass(),
      .framebuffer = _swapchain->current_framebuffer(),
      .renderArea = {
        .offset = { 0, 0 },
        .extent = _swapchain->extent()
      },
      .clearValueCount = 1,
      .pClearValues = &clear_color
    };

    vkCmdBeginRenderPass(_swapchain->current_command_buffer(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(_swapchain->current_command_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->handle());

    const auto& swapchain_extent = _swapchain->extent();

    const auto viewport = VkViewport {
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<sbx::float32>(swapchain_extent.width),
      .height = static_cast<sbx::float32>(swapchain_extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f
    };

    vkCmdSetViewport(_swapchain->current_command_buffer(), 0, 1, &viewport);

    const auto scissor = VkRect2D {
      .offset = {0, 0},
      .extent = swapchain_extent
    };

    vkCmdSetScissor(_swapchain->current_command_buffer(), 0, 1, &scissor);

    const auto buffers = std::array<VkBuffer, 1>{_vertex_buffer->handle()};
    const auto offsets = std::array<VkDeviceSize, 1>{0};

    vkCmdBindVertexBuffers(_swapchain->current_command_buffer(), 0, 1, buffers.data(), offsets.data());

    vkCmdDraw(_swapchain->current_command_buffer(), 3, 1, 0, 0);

    vkCmdEndRenderPass(_swapchain->current_command_buffer());

    if (vkEndCommandBuffer(_swapchain->current_command_buffer()) != VK_SUCCESS) {
      throw std::runtime_error("Failed to record command buffer");
    }
  }

  std::filesystem::path _config_path{};
  std::vector<std::string> _cli_args{};
  std::vector<subscription> _subscriptions{};

  bool _is_running{};
  bool _is_paused{};

  std::unique_ptr<configuration> _configuration{};
  std::unique_ptr<logger> _logger{};
  std::unique_ptr<event_manager> _event_manager{};

  std::unique_ptr<context> _context{};
  std::unique_ptr<monitor> _monitor{};
  std::unique_ptr<window> _window{};
  std::unique_ptr<input> _input{};

  std::unique_ptr<instance> _instance{};
  std::unique_ptr<surface> _surface{};
  std::unique_ptr<physical_device> _physical_device{};
  std::unique_ptr<logical_device> _logical_device{};
  std::unique_ptr<command_pool> _command_pool{};
  std::unique_ptr<swapchain> _swapchain{};
  std::unique_ptr<pipeline> _pipeline{};

  std::unique_ptr<buffer<vertex, 3>> _vertex_buffer{};

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
