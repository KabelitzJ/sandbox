#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <string>
#include <memory>
#include <iostream>
#include <chrono>

#include "logger.hpp"
#include "configuration.hpp"
#include "event_manager.hpp"
#include "window.hpp"
#include "pipeline.hpp"

#include "time.hpp"
#include "events.hpp"
#include "hashed_string.hpp"

namespace demo {

class application {

public:

  application(const std::filesystem::path& config_path)
  : _subscriptions{},
    _is_running{false},
    _is_paused{false},
    _logger{std::make_unique<logger>()},
    _configuration{std::make_unique<configuration>(config_path, _logger.get())},
    _event_manager{std::make_unique<event_manager>(_logger.get())},
    _window{std::make_unique<window>(_logger.get(), _configuration.get(), _event_manager.get())},
    // _device{std::make_unique<device>(_logger.get(), _configuration.get(), _window.get())},
    _pipeline{std::make_unique<pipeline>("demo/assets/shaders/basic", _logger.get())} { }

  ~application() {
    for (const auto& subscription : _subscriptions) {
      _event_manager->unsubscribe(subscription);
    }
  }

  void start() {
    _initialize();
    _run();
  }

private:

  void _initialize() {
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

  void _run() {
    using namespace demo::literals;

    using clock = std::chrono::high_resolution_clock;

    _is_running = true;

    auto start = clock::now();

    while (_is_running) {
      _window->poll_events();

      if (_is_paused) {
        continue;
      }

      const auto now = clock::now();
      const auto delta = time{now - start};
      start = now;
    }
  }

  std::vector<subscription> _subscriptions{};

  bool _is_running{};
  bool _is_paused{};

  std::unique_ptr<logger> _logger{};
  std::unique_ptr<configuration> _configuration{};
  std::unique_ptr<event_manager> _event_manager{};
  std::unique_ptr<window> _window{};
  // std::unique_ptr<device> _device{};
  std::unique_ptr<pipeline> _pipeline{};

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
