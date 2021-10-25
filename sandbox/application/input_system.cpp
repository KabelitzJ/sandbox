#include "input_system.hpp"

#include "events.hpp"

namespace sbx {

input_system::input_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle} { }

void input_system::initialize() {
  _event_queue->add_listener<window_closed_event>([this](const auto){
    finish();
  });

  glfwSetKeyCallback(_handle, [](auto* window, auto key, auto scancode, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));
    queue->emplace_back<key_event>(key, scancode, action, mods);
  });
}

void input_system::update(const time) {

}

void input_system::finished() {

}

void input_system::aborted() {

}

} // namespace sbx
