#ifndef SBX_EVTSYS_KEY_EVENT_HPP_
#define SBX_EVTSYS_KEY_EVENT_HPP_

#include "event.hpp"
#include "key_codes.hpp"

namespace sbx {

class key_pressed_event : public event {

public:
  key_pressed_event(key_code code);
  virtual ~key_pressed_event() = default;

  event_type type() const override;

  key_code code() const;

private:
  key_code _code;

}; // class key_event

class key_repeated_event : public event {

public:
  key_repeated_event(key_code code);
  virtual ~key_repeated_event() = default;

  event_type type() const override;

  key_code code() const;

private:
  key_code _code;

}; // class key_event

class key_released_event : public event {

public:
  key_released_event(key_code code);
  virtual ~key_released_event() = default;

  event_type type() const override;

  key_code code() const;

private:
  key_code _code;

}; // class key_event

} // namespace sbx

#endif // SBX_EVTSYS_KEY_EVENT_HPP_
