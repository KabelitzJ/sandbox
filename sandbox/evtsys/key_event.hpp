#ifndef SBX_EVTSYS_KEY_EVENT_HPP_
#define SBX_EVTSYS_KEY_EVENT_HPP_

#include "event.hpp"
#include "key_codes.hpp"

namespace sbx {

struct key_pressed_event : public event {

  key_pressed_event(key_code code) : code(code) {}
  virtual ~key_pressed_event() = default;

  key_code code;

}; // class key_event


struct key_repeated_event : public event {

public:
  key_repeated_event(key_code code) : code(code) {}
  virtual ~key_repeated_event() = default;

  key_code code;

}; // class key_event

/**
 * @brief An event that is fired when a key has been released
 */
struct key_released_event : public event {

  key_released_event(key_code code) : code(code) {}
  virtual ~key_released_event() = default;

  key_code code;

}; // class key_event

} // namespace sbx

#endif // SBX_EVTSYS_KEY_EVENT_HPP_
