#include "input_system.hpp"

#include <types/primitives.hpp>

#include <core/events.hpp>
#include <core/key.hpp>
#include <core/mouse_button.hpp>
#include <core/modification_flag.hpp>

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

  glfwSetKeyCallback(_handle, [](auto* window, auto keycode, auto scancode, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->emplace_back<key_pressed_event>(static_cast<key>(keycode), scancode, static_cast<key_modifiers>(mods));
    } else if (action == GLFW_RELEASE) {
      queue->emplace_back<key_released_event>(static_cast<key>(keycode), scancode, static_cast<key_modifiers>(mods));
    } else if (action == GLFW_REPEAT) {
      queue->emplace_back<key_repeated_event>(static_cast<key>(keycode), scancode, static_cast<key_modifiers>(mods));
    }
  });

  glfwSetMouseButtonCallback(_handle, [](auto* window, auto button, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->emplace_back<mouse_button_pressed_event>(static_cast<mouse_button>(button), static_cast<key_modifiers>(mods));
    } else if (action == GLFW_RELEASE) {
      queue->emplace_back<mouse_button_released_event>(static_cast<mouse_button>(button), static_cast<key_modifiers>(mods));
    } else if (action == GLFW_REPEAT) {
      queue->emplace_back<mouse_button_repeated_event>(static_cast<mouse_button>(button), static_cast<key_modifiers>(mods));
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

  glfwSetFramebufferSizeCallback(_handle, [](auto* window, auto width, auto height){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->emplace_back<window_resized_event>(width, height);
  });
}

void input_system::update([[maybe_unused]] const time delta_time) {

}

void input_system::finished() {

}

void input_system::aborted() {

}

} // namespace sbx
