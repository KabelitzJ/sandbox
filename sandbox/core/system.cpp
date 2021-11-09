#include "system.hpp"

namespace sbx {

system::system()
: _is_running{false} { }

[[nodiscard]] bool system::is_running() const noexcept {
  return _is_running;
}

void system::_initialize() {
  initialize();
  _is_running = true;
}

void system::_update(const time delta_time) {
  if (is_running()) {
    update(delta_time);
  }
}

void system::_terminate() {
  if (is_running()) {
    terminate();
    _is_running = false;
  }
}

} // namespace sbx
