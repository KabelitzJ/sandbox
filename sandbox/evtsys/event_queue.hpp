#ifndef SBX_CORE_EVENT_QUEUE_HPP_
#define SBX_CORE_EVENT_QUEUE_HPP_

#include <queue>
#include <vector>

#include <GLFW/glfw3.h>

#include "event.hpp"
#include "window_event_listener.hpp"
#include "key_event_listener.hpp"
#include "mouse_event_listener.hpp"

namespace sbx {

class event_queue {

public:
  event_queue(GLFWwindow* context);
  ~event_queue();

  void poll_events();

  template<typename Listener>
  void register_listener(Listener* listener);

private:
  static void window_moved_callback(GLFWwindow* window, int x, int y);
  static void window_resized_callback(GLFWwindow* window, int width, int height);
  static void window_closed_callback(GLFWwindow* window);
  static void window_refreshed_callback(GLFWwindow* window);
  static void framebuffer_resized_callback(GLFWwindow* window, int width, int height);
  static void key_callback(GLFWwindow* window, int key,int scancode, int action, int mods);

  void bind();

  GLFWwindow* _context;
  std::queue<event*> _queue;
  std::vector<window_event_listener*> _window_event_listeners;
  std::vector<key_event_listener*> _keyboard_event_listeners;
  std::vector<mouse_event_listener*> _mouse_event_listeners;

}; // class event_queue

template<typename Listener>
inline void event_queue::register_listener(Listener* listener) {
  if constexpr (std::is_base_of_v<window_event_listener, Listener>) {
    _window_event_listeners.push_back(listener);
  }

  if constexpr (std::is_base_of_v<key_event_listener, Listener>) {
    _keyboard_event_listeners.push_back(listener);
  }

  if constexpr (std::is_base_of_v<mouse_event_listener, Listener>) {
    _mouse_event_listeners.push_back(listener);
  }
}

} // namespace sbx

#endif // SBX_CORE_EVENT_QUEUE_HPP_
