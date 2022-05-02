#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <string>
#include <memory>
#include <iostream>

#include "pipeline.hpp"
#include "logger.hpp"
#include "configuration.hpp"
#include "event_manager.hpp"

namespace demo {


class application {

public:

  application(const std::filesystem::path& config_path)
  : _logger{std::make_unique<logger>()},
    _configuration{std::make_unique<configuration>(config_path, _logger.get())},
    _event_manager{std::make_unique<event_manager>(_logger.get())},
    // _window{std::make_unique<window>(_logger.get(), _configuration.get())},
    // _device{std::make_unique<device>(_logger.get(), _configuration.get(), _window.get())},
    _pipeline{std::make_unique<pipeline>("demo/assets/shaders/basic", _logger.get())} { }

  ~application() = default;

  void run() {
    // while (!_window->should_close()) {
    //   _window->poll_events();
    // }
  }

private:

  std::unique_ptr<logger> _logger{};
  std::unique_ptr<configuration> _configuration{};
  std::unique_ptr<event_manager> _event_manager{};
  // std::unique_ptr<window> _window{};
  // std::unique_ptr<device> _device{};
  std::unique_ptr<pipeline> _pipeline{};

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
