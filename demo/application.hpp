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
#include "swapchain.hpp"
#include "pipeline.hpp"

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
    _pipeline{nullptr} { }

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
    _pipeline = std::make_unique<pipeline>("demo/assets/shaders/basic");

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
      } else if (_input->is_key_pressed(key::p)) {
        _is_paused = !_is_paused;
      } else if (_input->is_key_pressed(key::f)) {
        _window->set_fullscreen();
      } else if (_input->is_key_pressed(key::r)) {
        _window->set_windowed();
      } else if (_input->is_button_pressed(button::left)) {
        _is_running = false;
      }

      if (_is_paused) {
        continue;
      }

      _window->swap_buffers();
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
  std::unique_ptr<pipeline> _pipeline{};

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
