#ifndef SBX_CORE_KEY_EVENT_LISTENER_HPP_
#define SBX_CORE_KEY_EVENT_LISTENER_HPP_

#include "key_codes.hpp"

namespace sbx {

class key_event_listener {

public:
  key_event_listener() = default;
  virtual ~key_event_listener() = default;

  virtual void on_key_pressed(key_code code) {}
  virtual void on_key_repeated(key_code code) {}
  virtual void on_key_released(key_code code) {}

}; // class key_event_listener

} // namespace sbx

#endif // SBX_CORE_KEY_EVENT_LISTENER_HPP_
