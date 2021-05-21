#include "event_queue.hpp"

#include <stdexcept>

#include "window_event.hpp"
#include "key_event.hpp"
#include "mouse_event.hpp"

namespace sbx {

event_queue::event_queue(GLFWwindow* context)
: _context(context),
  _queue(),
  _subscribers() {
  bind_callbacks();
}

event_queue::~event_queue() {
  unbind_callbacks();
}

void event_queue::poll() {
  glfwPollEvents();

  while (!_queue.empty()) {
    const auto [type, event] = std::move(_queue.front());
    _queue.pop();

    for (auto& subscriber : _subscribers[type]) {
      subscriber(*event);
    }
  }
}

void event_queue::bind_callbacks() {
  glfwSetWindowUserPointer(_context, this);

  glfwSetKeyCallback(_context, [](GLFWwindow* window, int key, int, int action, int) {
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    switch (action) {
      case GLFW_PRESS: {
        queue->push<key_pressed_event>(static_cast<key_code>(key));
        break;
      }
      case GLFW_REPEAT: {
        queue->push<key_repeated_event>(static_cast<key_code>(key));
        break;
      }
      case GLFW_RELEASE: {
        queue->push<key_released_event>(static_cast<key_code>(key));
        break;
      }
    };
  });

  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(_context, GLFW_RAW_MOUSE_MOTION, true);
  }

  glfwSetCursorPosCallback(_context, [](GLFWwindow* window, double xpos, double ypos){
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->push<mouse_moved_event>(static_cast<float>(xpos), static_cast<float>(ypos));
  });
}

void event_queue::unbind_callbacks() {
  glfwSetKeyCallback(_context, nullptr);

  glfwSetWindowUserPointer(_context, nullptr);
}

} // namespace sbx
