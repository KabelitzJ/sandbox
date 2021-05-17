#ifndef SBX_CORE_WINDOW_EVENT_LISTENER_HPP_
#define SBX_CORE_WINDOW_EVENT_LISTENER_HPP_

#include "window_event.hpp"

namespace sbx {

class window_event_listener {

public:
  window_event_listener() = default;
  virtual ~window_event_listener() = default;

private:
  virtual void on_window_event(window_event* event) = 0;

  friend class event_queue;

}; // class window_event_listener

} // namespace sbx

#endif // SBX_CORE_WINDOW_EVENT_LISTENER_HPP_
