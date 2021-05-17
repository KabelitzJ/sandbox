#ifndef SBX_CORE_MOUSE_EVENT_LISTENER_HPP_
#define SBX_CORE_MOUSE_EVENT_LISTENER_HPP_

#include "mouse_event.hpp"

namespace sbx {

class mouse_event_listener {

public:
  mouse_event_listener() = default;
  virtual ~mouse_event_listener() = default;

  virtual void on_mouse_event(mouse_event* event) = 0;

}; // class mouse_event_listener

} // namespace sbx

#endif // SBX_CORE_MOUSE_EVENT_LISTENER_HPP_
