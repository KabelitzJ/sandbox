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
  _bind_callbacks();
}

event_queue::~event_queue() {
  _unbind_callbacks();
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

void event_queue::_bind_callbacks() {
  glfwSetWindowUserPointer(_context, this);

  glfwSetKeyCallback(_context, [](GLFWwindow* window, int key, int, int action, int) {
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    switch (action) {
      case GLFW_PRESS: {
        queue->_push<key_pressed_event>(static_cast<key_code>(key));
        break;
      }
      case GLFW_REPEAT: {
        queue->_push<key_repeated_event>(static_cast<key_code>(key));
        break;
      }
      case GLFW_RELEASE: {
        queue->_push<key_released_event>(static_cast<key_code>(key));
        break;
      }
    };
  });

  glfwSetMouseButtonCallback(_context, [](GLFWwindow* window, int button ,int action ,int){
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    switch (action) {
      case GLFW_PRESS: {
        queue->_push<mouse_button_pressed_event>(static_cast<mouse_button>(button));
        break;
      }
      case GLFW_RELEASE: {
        queue->_push<mouse_button_released_event>(static_cast<mouse_button>(button));
        break;
      }
    }
  });

  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(_context, GLFW_RAW_MOUSE_MOTION, true);
  }

  glfwSetCursorPosCallback(_context, [](GLFWwindow* window, double xpos, double ypos){
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

    queue->_push<mouse_moved_event>(static_cast<float>(xpos), static_cast<float>(ypos));
  });
}

void event_queue::_unbind_callbacks() {
  glfwSetKeyCallback(_context, nullptr);
  glfwSetCursorPosCallback(_context, nullptr);
  glfwSetMouseButtonCallback(_context, nullptr);

  glfwSetWindowUserPointer(_context, nullptr);
}

} // namespace sbx
