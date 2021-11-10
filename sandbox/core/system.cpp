#include "system.hpp"

namespace sbx {

system::system()
: _is_running{false} { }

[[nodiscard]] bool system::is_running() const noexcept {
  return _is_running;
}

entity system::create_entity(const entity parent) {
  assert(_scene); // Scene is uninitialized
  return _scene->create_entity(parent);
}

void system::destroy_entity(const entity entity) {
  assert(_scene); // Scene is uninitialized
  _scene->destroy_entity(entity);
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
