#include "input_system.hpp"

#include <types/primitives.hpp>

#include <core/events.hpp>
#include <core/key.hpp>
#include <core/mouse_button.hpp>
#include <core/input_modifiers.hpp>

namespace sbx {

input_system::input_system(GLFWwindow* handle)
: _handle{handle} { }

void input_system::initialize() {
  glfwSetWindowCloseCallback(_handle, [](auto* window){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->dispatch_event<window_closed_event>("window close callback");
  });

  glfwSetKeyCallback(_handle, [](auto* window, auto keycode, auto scancode, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->dispatch_event<key_pressed_event>(static_cast<key>(keycode), scancode, static_cast<modifiers>(mods));
    } else if (action == GLFW_RELEASE) {
      queue->dispatch_event<key_released_event>(static_cast<key>(keycode), scancode, static_cast<modifiers>(mods));
    } else if (action == GLFW_REPEAT) {
      queue->dispatch_event<key_repeated_event>(static_cast<key>(keycode), scancode, static_cast<modifiers>(mods));
    }
  });

  glfwSetMouseButtonCallback(_handle, [](auto* window, auto button, auto action, auto mods){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS) {
      queue->dispatch_event<mouse_button_pressed_event>(static_cast<mouse_button>(button), static_cast<modifiers>(mods));
    } else if (action == GLFW_RELEASE) {
      queue->dispatch_event<mouse_button_released_event>(static_cast<mouse_button>(button), static_cast<modifiers>(mods));
    } else if (action == GLFW_REPEAT) {
      queue->dispatch_event<mouse_button_repeated_event>(static_cast<mouse_button>(button), static_cast<modifiers>(mods));
    }
  });

  glfwSetCursorPosCallback(_handle, [](auto* window, auto x_position, auto y_position){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->dispatch_event<mouse_moved_event>(static_cast<float32>(x_position), static_cast<float32>(y_position));
  });

  glfwSetScrollCallback(_handle, [](auto* window, auto x_offset, auto y_offset){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->dispatch_event<scroll_event>(static_cast<float32>(x_offset), static_cast<float32>(y_offset));
  });

  glfwSetFramebufferSizeCallback(_handle, [](auto* window, auto width, auto height){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->dispatch_event<window_resized_event>(width, height);
  });

  glfwSetWindowFocusCallback(_handle, [](auto* window, auto focused){
    auto* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->dispatch_event<window_focused_event>(focused == GLFW_TRUE);
  });
}

void input_system::update([[maybe_unused]] const time delta_time) {

}

void input_system::terminate() {

}

} // namespace sbx
