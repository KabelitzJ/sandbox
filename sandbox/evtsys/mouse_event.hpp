#ifndef SBX_EVTSYS_MOUSE_EVENT_HPP_
#define SBX_EVTSYS_MOUSE_EVENT_HPP_

#include "event.hpp"
#include "mouse_buttons.hpp"

namespace sbx {

class mouse_moved_event : public event {

public:
  mouse_moved_event(float x, float y);
  ~mouse_moved_event() = default;

  event_type type() const override;

  float x() const;
  float y() const;

private:
  float _x;
  float _y;

}; // class mouse_moved_event

class mouse_button_pressed_event : public event {

public:
  mouse_button_pressed_event(mouse_button button);
  ~mouse_button_pressed_event() = default;

  event_type type() const override;

  mouse_button button() const;

private:
  mouse_button _button;

}; // class mouse_button_pressed_event

class mouse_button_released_event : public event {

public:
  mouse_button_released_event(mouse_button button);
  ~mouse_button_released_event() = default;

  event_type type() const override;

  mouse_button button() const;

private:
  mouse_button _button;

}; // class mouse_button_released_event

} // namespace sbx

#endif // SBX_EVTSYS_MOUSE_EVENT_HPP_
