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
#include "monitor.hpp"
#include "window.hpp"
#include "input.hpp"

#include "instance.hpp"
#include "physical_device.hpp"
#include "logical_device.hpp"
#include "surface.hpp"
#include "render_pass.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "framebuffers.hpp"
#include "command_pool.hpp"
#include "command_buffer.hpp"
#include "semaphore.hpp"
#include "fence.hpp"

#include "time.hpp"
#include "events.hpp"
#include "hashed_string.hpp"
#include "key.hpp"

namespace demo {

class application {

public:

  /**
   * @brief Construct a new application
   * 
   * @param configuration The path to the configuration file
   */
  application(const std::filesystem::path& config_path)
  : _config_path{config_path},
    _subscriptions{},
    _is_running{false},
    _is_paused{false},
    _logger{nullptr},
    _configuration{nullptr},
    _event_manager{nullptr},
    _monitor{nullptr},
    _window{nullptr},
    _input{nullptr},
    _instance{nullptr},
    _surface{nullptr},
    _physical_device{nullptr},
    _logical_device{nullptr},
    _swapchain{nullptr},
    _render_pass{nullptr},
    _pipeline{nullptr},
    _framebuffers{nullptr},
    _command_pool{nullptr},
    _command_buffer{nullptr},
    _image_available_semaphore{nullptr},
    _render_finished_semaphore{nullptr},
    _image_in_flight_fence{nullptr} { }

  ~application() {
    for (const auto& subscription : _subscriptions) {
      _event_manager->unsubscribe(subscription);
    }
  }

  /**
   * @brief Initializes and runs the application.
   * 
   * @returns EXIT_SUCCESS if the application ran successfully, EXIT_FAILURE if an error occurred.
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
   * @brief Initializes all systems of the application.
   */
  void _initialize() {
    // Set up all systems - ORDER MATTERS because of dependencies

    // Core systems
    _logger = std::make_unique<logger>();
    _configuration = std::make_unique<configuration>(_config_path);
    _event_manager = std::make_unique<event_manager>(_logger.get());

    // Window related systems
    _monitor = std::make_unique<monitor>(_event_manager.get());
    _window = std::make_unique<window>(_configuration.get(), _event_manager.get(), _monitor.get());
    _input = std::make_unique<input>(_event_manager.get());

    // Vulkan related systems
    _instance = std::make_unique<instance>(_logger.get(), _window.get(), _configuration.get());
    _surface = std::make_unique<surface>(_window.get(), _instance.get());
    _physical_device = std::make_unique<physical_device>(_logger.get(), _instance.get(), _surface.get());
    _logical_device = std::make_unique<logical_device>(_instance.get(), _physical_device.get());
    _swapchain = std::make_unique<swapchain>(_window.get(), _surface.get(), _physical_device.get(), _logical_device.get());
    _render_pass = std::make_unique<render_pass>(_logical_device.get(), _swapchain.get());
    _pipeline = std::make_unique<pipeline>("demo/assets/shaders/basic", _logical_device.get(), _swapchain.get(), _render_pass.get());
    _framebuffers = std::make_unique<framebuffers>(_logical_device.get(), _swapchain.get(), _render_pass.get());
    _command_pool = std::make_unique<command_pool>(_physical_device.get(), _logical_device.get());
    _command_buffer = std::make_unique<command_buffer>(_logical_device.get(), _command_pool.get());
    _image_available_semaphore = std::make_unique<semaphore>(_logical_device.get());
    _render_finished_semaphore = std::make_unique<semaphore>(_logical_device.get());
    _image_in_flight_fence = std::make_unique<fence>(_logical_device.get(), VK_FENCE_CREATE_SIGNALED_BIT);

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
   * @brief Runs the application.
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

      _draw_frame();

      // _window->swap_buffers();
    }

    vkDeviceWaitIdle(_logical_device->handle());
  }

  void _draw_frame() {
    const auto image_in_flight_fence_handle = _image_in_flight_fence->handle();
    const auto image_available_semaphore_handle = _image_available_semaphore->handle();
    const auto render_finished_semaphore_handle = _render_finished_semaphore->handle();
    const auto command_buffer_handle = _command_buffer->handle();
    const auto swapchain_handle = _swapchain->handle();

    vkWaitForFences(_logical_device->handle(), 1, &image_in_flight_fence_handle, VK_TRUE, std::numeric_limits<sbx::uint64>::max());
    vkResetFences(_logical_device->handle(), 1, &image_in_flight_fence_handle);

    auto image_index = sbx::uint32{0};
    vkAcquireNextImageKHR(_logical_device->handle(), _swapchain->handle(), std::numeric_limits<sbx::uint64>::max(), image_available_semaphore_handle, nullptr, &image_index);

    vkResetCommandBuffer(command_buffer_handle, 0);

    _record_command_buffer(image_index);

    const auto waitStages = std::array<VkPipelineStageFlags, 1>{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    const auto submit_info = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &image_available_semaphore_handle,
      .pWaitDstStageMask = waitStages.data(),
      .commandBufferCount = 1,
      .pCommandBuffers = &command_buffer_handle,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &render_finished_semaphore_handle
    }; 

    if (vkQueueSubmit(_logical_device->graphics_queue(), 1, &submit_info, image_in_flight_fence_handle) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer");
    }

    const auto present_info = VkPresentInfoKHR{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &render_finished_semaphore_handle,
      .swapchainCount = 1,
      .pSwapchains = &swapchain_handle,
      .pImageIndices = &image_index,
      .pResults = nullptr
    };

    vkQueuePresentKHR(_logical_device->present_queue(), &present_info);
  }

  void _record_command_buffer(const sbx::uint32 image_index) {
    const auto command_buffer_begin_info = VkCommandBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr
    };
    
    if (vkBeginCommandBuffer(_command_buffer->handle(), &command_buffer_begin_info) != VK_SUCCESS) {
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
      .renderPass = _render_pass->handle(),
      .framebuffer = (*_framebuffers.get())[image_index],
      .renderArea = {
        .offset = { 0, 0 },
        .extent = _swapchain->extent()
      },
      .clearValueCount = 1,
      .pClearValues = &clear_color
    };

    vkCmdBeginRenderPass(_command_buffer->handle(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(_command_buffer->handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline->handle());

    vkCmdDraw(_command_buffer->handle(), 3, 1, 0, 0);

    vkCmdEndRenderPass(_command_buffer->handle());

    if (vkEndCommandBuffer(_command_buffer->handle()) != VK_SUCCESS) {
      throw std::runtime_error("Failed to record command buffer");
    }
  }

  std::filesystem::path _config_path{};
  std::vector<subscription> _subscriptions{};

  bool _is_running{};
  bool _is_paused{};

  std::unique_ptr<logger> _logger{};
  std::unique_ptr<configuration> _configuration{};
  std::unique_ptr<event_manager> _event_manager{};
  std::unique_ptr<monitor> _monitor{};
  std::unique_ptr<window> _window{};
  std::unique_ptr<input> _input{};
  std::unique_ptr<instance> _instance{};
  std::unique_ptr<surface> _surface{};
  std::unique_ptr<physical_device> _physical_device{};
  std::unique_ptr<logical_device> _logical_device{};
  std::unique_ptr<swapchain> _swapchain{};
  std::unique_ptr<render_pass> _render_pass{};
  std::unique_ptr<pipeline> _pipeline{};
  std::unique_ptr<framebuffers> _framebuffers{};
  std::unique_ptr<command_pool> _command_pool{};
  std::unique_ptr<command_buffer> _command_buffer{};
  std::unique_ptr<semaphore> _image_available_semaphore{};
  std::unique_ptr<semaphore> _render_finished_semaphore{};
  std::unique_ptr<fence> _image_in_flight_fence{};

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
