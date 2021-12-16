#include "system.hpp"

namespace sbx {

system::system()
: _is_running{false} { }

[[nodiscard]] bool system::is_running() const noexcept {
  return _is_running;
}

entity system::create_entity(const transform& transform, const entity parent) {
  assert(_scene); // Scene is uninitialized
  return _scene->create_entity(transform, parent);
}

void system::destroy_entity(const entity entity) {
  assert(_scene); // Scene is uninitialized
  _scene->destroy_entity(entity);
}

bool system::is_key_down(const key key) const {
  assert(_input); // Input is uninitialized
  return _input->is_key_down(key);
}

bool system::is_key_up(const key key) const {
  assert(_input); // Input is uninitialized
  return _input->is_key_up(key);
}

bool system::is_mouse_button_down(const mouse_button button) const {
  assert(_input); // Input is uninitialized
  return _input->is_mouse_button_down(button);
}

bool system::is_mouse_button_up(const mouse_button button) const {
  assert(_input); // Input is uninitialized
  return _input->is_mouse_button_up(button);
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
