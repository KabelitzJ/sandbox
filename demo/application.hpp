#ifndef DEMO_APPLICATION_HPP_
#define DEMO_APPLICATION_HPP_

#include <string>

#include "window.hpp"
#include "device.hpp"
#include "pipeline.hpp"

namespace demo {

class application {

public:

  inline static constexpr auto width = sbx::int32{800};
  inline static constexpr auto height = sbx::int32{600};

  application(const std::string& name)
  : _window{name, width, height},
    _device{_window, name},
    _pipeline{"demo/assets/shaders/basic"} { }

  ~application() = default;

  void run() {
    while (!_window.should_close()) {
      _window.poll_events();
    }
  }

private:

  window _window;
  device _device;
  pipeline _pipeline;

}; // class application

} // namespace demo

#endif // DEMO_APPLICATION_HPP_
