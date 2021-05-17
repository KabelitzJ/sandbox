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

  void register_window_event_listener(window_event_listener* listener);
  void register_key_event_listener(key_event_listener* listener);
  void register_mouse_event_listener(mouse_event_listener* listener);

private:
  static void window_position_callback(GLFWwindow* window, int x, int y);
  static void window_size_callback(GLFWwindow* window, int width, int height);
  static void window_close_callback(GLFWwindow* window);
  static void window_framebuffer_size_callback(GLFWwindow* window, int width, int height);

  void bind();

  GLFWwindow* _context;
  std::queue<event*> _queue;
  std::vector<window_event_listener*> _window_event_listeners;
  std::vector<key_event_listener*> _key_event_listeners;
  std::vector<mouse_event_listener*> _mouse_event_listeners;

}; // class event_queue

} // namespace sbx

#endif // SBX_CORE_EVENT_QUEUE_HPP_
