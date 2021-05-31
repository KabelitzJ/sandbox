#ifndef SBX_EVTSYS_WINDOW_EVENT_HPP_
#define SBX_EVTSYS_WINDOW_EVENT_HPP_

#include "event.hpp"

namespace sbx {

struct window_moved_event : public event {

  window_moved_event(int x, int y) : x(x), y(y) {}
  ~window_moved_event() = default;

  int x;
  int y;

}; // class window_moved_event


struct window_resized_event : public event {

  window_resized_event(int width, int height) : width(width), height(height) {}
  ~window_resized_event() = default;

  int width;
  int height;

}; // class window_resized_event


struct window_closed_event : public event {

  window_closed_event() = default;
  ~window_closed_event() = default;

}; // class window_closed_event


struct window_refreshed_event : public event {

  window_refreshed_event() = default;
  ~window_refreshed_event() = default;

}; // class window_refreshed_event

/**
 * @brief An event that is fired when opengl registers a framebuffer resize 
 */
struct framebuffer_resized_event : public event {

  framebuffer_resized_event(int width, int height) : width(width), height(height) {}
  ~framebuffer_resized_event() = default;

  int width;
  int height;

}; // class framebuffer_resized_event

} // namespace sbx

#endif // SBX_EVTSYS_WINDOW_EVENT_HPP_
