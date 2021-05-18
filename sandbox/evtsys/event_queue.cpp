#include "event_queue.hpp"

#include <stdexcept>

#include "window_event.hpp"

namespace sbx {

event_queue::event_queue(GLFWwindow* context)
: _context(context),
  _queue(),
  _window_event_listeners(),
  _keyboard_event_listeners(),
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

    if (base_event->is_in_category(event_category::KEYBOARD)) {
      for (auto* listener : _keyboard_event_listeners) {
        switch (base_event->type()) {
          case event_type::KEY_PRESSED: {
            listener->on_key_pressed(1);
            break;
          }
          case event_type::KEY_REPEATED: {
            listener->on_key_repeated(1);
            break;
          }
          case event_type::KEY_RELEASED: {
            listener->on_key_released(1);
            break;
          }
        }
      }
    }

    delete base_event;
  }
}

void event_queue::window_moved_callback(GLFWwindow* window, int x, int y) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new window_moved_event(x, y);

  queue._queue.push(event);
}

void event_queue::window_resized_callback(GLFWwindow* window, int width, int height) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new window_resized_event(width, height);

  queue._queue.push(event);
}

void event_queue::window_closed_callback(GLFWwindow* window) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new window_closed_event();

  queue._queue.push(event);
}

void event_queue::window_refreshed_callback(GLFWwindow* window) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new window_refreshed_event();

  queue._queue.push(event);
}

void event_queue::framebuffer_resized_callback(GLFWwindow* window, int width, int height) {
  event_queue queue = *static_cast<event_queue*>(glfwGetWindowUserPointer(window));

  event* event = new framebuffer_resized_event(width, height);

  queue._queue.push(event);
}

void event_queue::bind() {
  // KEY_PRESSED    
  // KEY_REPEATED       
  // KEY_RELEASED    

  // BUTTON_PRESSED     
  // BUTTON_RELEASED    
  // CURSOR_MOVED       
  // CURSOR_ENTERED     
  // CURSOR_LEFT        
  // SCROLLED     

  // WINDOW_MOVED       
  glfwSetWindowPosCallback(_context, &event_queue::window_moved_callback);
  // WINDOW_RESIZED     
  glfwSetWindowSizeCallback(_context, &event_queue::window_resized_callback);
  // WINDOW_CLOSED      
  glfwSetWindowCloseCallback(_context, &event_queue::window_closed_callback);
  // WINDOW_REFRESHED   
  glfwSetWindowRefreshCallback(_context, &event_queue::window_refreshed_callback);
  // WINDOW_FOCUSED     
  // WINDOW_UNFOCUSED   
  // WINDOW_ICONIFIED   
  // WINDOW_UNICONIFIED 
  // FRAMEBUFFER_RESIZED
  glfwSetFramebufferSizeCallback(_context, &event_queue::framebuffer_resized_callback);
}

} // namespace sbx
