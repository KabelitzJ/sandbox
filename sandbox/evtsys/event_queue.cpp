#include "event_queue.hpp"

#include <stdexcept>

#include "window_event.hpp"

namespace sbx {

event_queue::event_queue(GLFWwindow* context)
: _context(context),
  _queue(),
  _window_event_listeners(),
  _key_event_listeners(),
  _mouse_event_listeners() {
  if (glfwGetWindowUserPointer(_context)) {
    throw std::runtime_error("[Error] Context already has an event queue attached!\n");
  }
  glfwSetWindowUserPointer(_context, this);
  bind();
}

event_queue::~event_queue() {
  glfwSetWindowUserPointer(_context, nullptr);
}

void event_queue::poll_events() {
  glfwPollEvents();

  while (!_queue.empty()) {
    event* base_event = _queue.front();
    _queue.pop();

    switch (base_event->type()) {
      case event_type::window: {
        window_event* event = static_cast<window_event*>(base_event);

        for (auto* listener : _window_event_listeners) {
          listener->on_window_event(event);
        }
      }
      case event_type::keyboard: {

      }
      case event_type::mouse: {

      }
    }

    delete base_event;
  }
}

void event_queue::register_window_event_listener(window_event_listener* listener) {
  _window_event_listeners.push_back(listener);
}

void event_queue::register_key_event_listener(key_event_listener* listener) {
  _key_event_listeners.push_back(listener);
}

void event_queue::register_mouse_event_listener(mouse_event_listener* listener) {
  _mouse_event_listeners.push_back(listener);
}

void event_queue::window_position_callback(GLFWwindow* window, int x, int y) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new window_position_event(x, y);

  queue._queue.push(event);
}

void event_queue::window_size_callback(GLFWwindow* window, int width, int height) {

}

void event_queue::window_close_callback(GLFWwindow* window) {

}

void event_queue::window_framebuffer_size_callback(GLFWwindow* window, int width, int height) {

}

void event_queue::bind() {
  glfwSetWindowPosCallback(_context, &event_queue::window_position_callback);
  glfwSetWindowSizeCallback(_context, &event_queue::window_size_callback);
  glfwSetWindowCloseCallback(_context, &event_queue::window_close_callback);
  glfwSetFramebufferSizeCallback(_context, &event_queue::window_framebuffer_size_callback);
}

} // namespace sbx
