#ifndef SBX_APPLICATION_EVENTS_HPP_
#define SBX_APPLICATION_EVENTS_HPP_

#include <string_view>

#include <ecs/entity.hpp>

#include <types/primitives.hpp>
#include <types/vector.hpp>
#include <types/color.hpp>

#include "key.hpp"
#include "mouse_button.hpp"
#include "input_modifiers.hpp"

namespace sbx {

struct window_resized_event {
  int32 width{};
  int32 height{};
};

struct window_closed_event {
  std::string_view reason{};
};

struct window_focused_event {
  bool has_focus{};
};

struct key_pressed_event {
  key keycode{};
  int32 scancode{};
  modifiers mods{};
};

struct key_released_event {
  key keycode{};
  int32 scancode{};
  modifiers mods{};
};

struct key_repeated_event {
  key keycode{};
  int32 scancode{};
  modifiers mods{};
};

struct mouse_button_pressed_event {
  mouse_button button{};
  modifiers mods{};
};

struct mouse_button_released_event {
  mouse_button button{};
  modifiers mods{};
};

struct mouse_button_repeated_event {
  mouse_button button{};
  modifiers mods{};
};

struct mouse_moved_event {
  vector2 position{};
};

struct scroll_event {
  vector2 offset{};
};

struct set_mouse_visibility_event {
  bool is_visible{};
};

struct fps_updated_event {
  uint32 fps{};
};

struct clear_color_changed_event {
  color color{};
};

struct collision_event {
  entity first{};
  entity second{};
}; // struct collision_event

struct main_camera_changed_event {
  entity camera{};
};

} // namespace sbx

#endif // SBX_APPLICATION_EVENTS_HPP_
