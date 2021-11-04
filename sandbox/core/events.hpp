#ifndef SBX_APPLICATION_EVENTS_HPP_
#define SBX_APPLICATION_EVENTS_HPP_

#include <string_view>

#include <types/primitives.hpp>

#include <core/key.hpp>
#include <core/mouse_button.hpp>
#include <core/modification_flag.hpp>

namespace sbx {

struct application_shutdown_event {
  std::string_view origin{};
};

struct window_resized_event {
  int32 width{};
  int32 height{};
};



struct key_pressed_event {
  key key{};
  int32 scancode{};
  key_modifiers mods{};
};

struct key_released_event {
  key key{};
  int32 scancode{};
  key_modifiers mods{};
};

struct key_repeated_event {
  key key{};
  int32 scancode{};
  key_modifiers mods{};
};

struct mouse_button_pressed_event {
  mouse_button button{};
  key_modifiers mods{};
};

struct mouse_button_released_event {
  mouse_button button{};
  key_modifiers mods{};
};

struct mouse_button_repeated_event {
  mouse_button button{};
  key_modifiers mods{};
};

struct mouse_moved_event {
  float32 x{};
  float32 y{};
};

struct scroll_event {
  float32 x{};
  float32 y{};
};

struct toggle_mouse_visibility_event { };

struct fps_updated_event {
  uint32 fps{};
};

} // namespace sbx

#endif // SBX_APPLICATION_EVENTS_HPP_
