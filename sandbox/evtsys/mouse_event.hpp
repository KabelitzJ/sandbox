#ifndef SBX_EVTSYS_MOUSE_EVENT_HPP_
#define SBX_EVTSYS_MOUSE_EVENT_HPP_

#include "event.hpp"
#include "mouse_buttons.hpp"

namespace sbx {

struct mouse_moved_event : public event {

  mouse_moved_event(float x, float y) : x(x), y(y) {}
  ~mouse_moved_event() = default;

  float x;
  float y;

}; // class mouse_moved_event


struct mouse_button_pressed_event : public event {

  mouse_button_pressed_event(mouse_button button) : button(button) {}
  ~mouse_button_pressed_event() = default;

  mouse_button button;

}; // class mouse_button_pressed_event


struct mouse_button_released_event : public event {

  mouse_button_released_event(mouse_button button) : button(button) {}
  ~mouse_button_released_event() = default;

  mouse_button button;

}; // class mouse_button_released_event

} // namespace sbx

#endif // SBX_EVTSYS_MOUSE_EVENT_HPP_
