#ifndef SBX_CORE_KEY_EVENT_LISTENER_HPP_
#define SBX_CORE_KEY_EVENT_LISTENER_HPP_

#include "key_event.hpp"

namespace sbx {

class key_event_listener {

public:
  key_event_listener() = default;
  virtual ~key_event_listener() = default;

  virtual void on_key_event(key_event* event) = 0;

}; // class key_event_listener

} // namespace sbx

#endif // SBX_CORE_KEY_EVENT_LISTENER_HPP_
