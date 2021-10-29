#ifndef SBX_APPLICATION_EVENTS_HPP_
#define SBX_APPLICATION_EVENTS_HPP_

#include <types/primitives.hpp>

#include "input_codes.hpp"

namespace sbx {

struct window_closed_event { };

struct key_pressed_event {

  key_pressed_event(key _key, int32 _scancode, key_modifiers _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  key key{};
  int32 scancode{};
  key_modifiers mods{};

};

struct key_released_event {

  key_released_event(key _key, int32 _scancode, key_modifiers _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  key key{};
  int32 scancode{};
  key_modifiers mods{};

};

struct key_repeated_event {

  key_repeated_event(key _key, int32 _scancode, key_modifiers _mods)
  : key{_key},
    scancode{_scancode},
    mods{_mods} { }

  key key{};
  int32 scancode{};
  key_modifiers mods{};

};

struct mouse_button_pressed_event {

  mouse_button_pressed_event(mouse_button _button, key_modifiers _mods)
  : button{_button},
    mods{_mods} { }

  mouse_button button{};
  key_modifiers mods{};

};

struct mouse_button_released_event {

  mouse_button_released_event(mouse_button _button, key_modifiers _mods)
  : button{_button},
    mods{_mods} { }

  mouse_button button{};
  key_modifiers mods{};

};

struct mouse_button_repeated_event {

  mouse_button_repeated_event(mouse_button _button, key_modifiers _mods)
  : button{_button},
    mods{_mods} { }

  mouse_button button{};
  key_modifiers mods{};

};

struct mouse_moved_event {

  mouse_moved_event(float32 _x, float32 _y)
  : x{_x},
    y{_y} { }

  float32 x{};
  float32 y{};

};

struct scroll_event {

  scroll_event(float32 _x, float32 _y)
  : x{_x},
    y{_y} { }

  float32 x{};
  float32 y{};

};

struct toggle_mouse_visibility_event { };

struct fps_updated_event {

  fps_updated_event(uint32 _fps)
  : fps{_fps} { }

  uint32 fps{};

};

} // namespace sbx

#endif // SBX_APPLICATION_EVENTS_HPP_
