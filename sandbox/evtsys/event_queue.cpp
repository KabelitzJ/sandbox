#include "event_queue.hpp"

#include <stdexcept>

#include "key_event.hpp"
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
  glfwSetWindowUserPointer(_context, static_cast<void*>(&_queue));
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
            listener->on_key_pressed(key_code::SPACE);
            break;
          }
          case event_type::KEY_REPEATED: {
            listener->on_key_repeated(key_code::SPACE);
            break;
          }
          case event_type::KEY_RELEASED: {
            listener->on_key_released(key_code::SPACE);
            break;
          }
        }
      }
    }

    delete base_event;
  }
}

void event_queue::key_callback(GLFWwindow* window, int key,int scancode, int action, int mods) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = nullptr;

  switch (action) {
    case GLFW_PRESS: {
      event = new key_pressed_event(static_cast<key_code>(key));
      break;
    }
    case GLFW_REPEAT: {
      event = new key_repeated_event(static_cast<key_code>(key));
      break;
    }
    case GLFW_RELEASE: {
      event = new key_released_event(static_cast<key_code>(key));
      break;
    }
  }

  queue.push(event);
}

void event_queue::window_moved_callback(GLFWwindow* window, int x, int y) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = new window_moved_event(x, y);

  queue.push(event);
}

void event_queue::window_resized_callback(GLFWwindow* window, int width, int height) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = new window_resized_event(width, height);

  queue.push(event);
}

void event_queue::window_closed_callback(GLFWwindow* window) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = new window_closed_event();

  queue.push(event);
}

void event_queue::window_refreshed_callback(GLFWwindow* window) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = new window_refreshed_event();

  queue.push(event);
}

void event_queue::framebuffer_resized_callback(GLFWwindow* window, int width, int height) {
  std::queue<event*> queue = *static_cast<std::queue<event*>*>(glfwGetWindowUserPointer(window));

  event* event = new framebuffer_resized_event(width, height);

  queue.push(event);
}

void event_queue::bind() {
  // KEY_PRESSED    
  // KEY_REPEATED       
  // KEY_RELEASED
  glfwSetKeyCallback(_context, &event_queue::key_callback);

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
