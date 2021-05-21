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
  glfwSetWindowUserPointer(_context, static_cast<void*>(this));
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
            key_pressed_event* event = static_cast<key_pressed_event*>(base_event);
            listener->on_key_pressed(event->code());
            break;
          }
          case event_type::KEY_REPEATED: {
            key_pressed_event* event = static_cast<key_pressed_event*>(base_event);
            listener->on_key_repeated(event->code());
            break;
          }
          case event_type::KEY_RELEASED: {
            key_pressed_event* event = static_cast<key_pressed_event*>(base_event);
            listener->on_key_released(event->code());
            break;
          }
        }
      }
    }

    delete base_event;
  }
}

void event_queue::bind() {
  glfwSetWindowUserPointer(_context, this);

  // KEY_PRESSED    
  // KEY_REPEATED       
  // KEY_RELEASED
  glfwSetKeyCallback(_context, [](GLFWwindow* window, int key,int scancode, int action, int mods){
    event_queue* queue = static_cast<event_queue*>(glfwGetWindowUserPointer(window));

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

    queue->_queue.push(event);
  });

  // BUTTON_PRESSED     
  // BUTTON_RELEASED    
  // CURSOR_MOVED       
  // CURSOR_ENTERED     
  // CURSOR_LEFT        
  // SCROLLED     

  // WINDOW_MOVED       
  // WINDOW_RESIZED     
  // WINDOW_CLOSED      
  // WINDOW_REFRESHED   
  // WINDOW_FOCUSED     
  // WINDOW_UNFOCUSED   
  // WINDOW_ICONIFIED   
  // WINDOW_UNICONIFIED 
  // FRAMEBUFFER_RESIZED
}

void event_queue::unbind() {
  glfwSetWindowUserPointer(_context, nullptr);
}

} // namespace sbx
