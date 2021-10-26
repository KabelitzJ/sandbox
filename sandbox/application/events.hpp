#ifndef SBX_APPLICATION_EVENTS_HPP_
#define SBX_APPLICATION_EVENTS_HPP_

#include <types/primitives.hpp>

namespace sbx {

struct window_closed_event { };

struct key_pressed_event {

  key_pressed_event(int32 _key, int32 _scancode, int32 _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  int32 key;
  int32 scancode;
  int32 mods;

};

struct key_released_event {

  key_released_event(int32 _key, int32 _scancode, int32 _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  int32 key;
  int32 scancode;
  int32 mods;

};

struct key_repeated_event {

  key_repeated_event(int32 _key, int32 _scancode, int32 _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  int32 key;
  int32 scancode;
  int32 mods;

};

struct mouse_button_pressed_event {

  mouse_button_pressed_event(int32 _button, int32 _mods)
  : button{_button},
    mods{_mods} { }

  int32 button;
  int32 mods;

};

struct mouse_button_released_event {

  mouse_button_released_event(int32 _button, int32 _mods)
  : button{_button},
    mods{_mods} { }

  int32 button;
  int32 mods;

};

struct mouse_button_repeated_event {

  mouse_button_repeated_event(int32 _button, int32 _mods)
  : button{_button},
    mods{_mods} { }

  int32 button;
  int32 mods;

};

struct mouse_moved_event {

  mouse_moved_event(float32 _x, float32 _y)
  : x{_x},
    y{_y} { }

  float32 x;
  float32 y;

};

struct scroll_event {

  scroll_event(float32 _x, float32 _y)
  : x{_x},
    y{_y} { }

  float32 x;
  float32 y;

};

} // namespace sbx

#endif // SBX_APPLICATION_EVENTS_HPP_
