#include "input_system.hpp"

#include <types/primitives.hpp>

#include "events.hpp"
#include "keys.hpp"

namespace sbx {

input_system::input_system(event_queue* event_queue, GLFWwindow* handle)
: _event_queue{event_queue},
  _handle{handle} { }

void input_system::initialize() {
  _event_queue->add_listener<window_closed_event>([this](const auto&){
    finish();
  });

  glfwSetWindowCloseCallback(_handle, [](auto* window){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));
    queue->emplace_back<window_closed_event>();
  });

  glfwSetKeyCallback(_handle, [](auto* window, auto key, auto scancode, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->emplace_back<key_pressed_event>(key, scancode, mods);
    } else if (action == GLFW_RELEASE) {
      queue->emplace_back<key_released_event>(key, scancode, mods);
    } else if (action == GLFW_REPEAT) {
      queue->emplace_back<key_repeated_event>(key, scancode, mods);
    }
  });

  glfwSetMouseButtonCallback(_handle, [](auto* window, auto button, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->emplace_back<mouse_button_pressed_event>(button, mods);
    } else if (action == GLFW_RELEASE) {
      queue->emplace_back<mouse_button_released_event>(button, mods);
    } else if (action == GLFW_REPEAT) {
      queue->emplace_back<mouse_button_repeated_event>(button, mods);
    }
  });

  glfwSetCursorPosCallback(_handle, [](auto* window, auto x_position, auto y_position){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->emplace_back<mouse_moved_event>(static_cast<float32>(x_position), static_cast<float32>(y_position));
  });

  glfwSetScrollCallback(_handle, [](auto* window, auto x_offset, auto y_offset){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->emplace_back<scroll_event>(static_cast<float32>(x_offset), static_cast<float32>(y_offset));
  });
}

void input_system::update([[maybe_unused]] const time delta_time) {

}

void input_system::finished() {

}

void input_system::aborted() {

}

} // namespace sbx
