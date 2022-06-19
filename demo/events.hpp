#ifndef SBX_DEMO_EVENTS_HPP_
#define SBX_DEMO_EVENTS_HPP_

#include <types/primitives.hpp>

#include "key.hpp"
#include "button.hpp"
#include "modifiers.hpp"

namespace demo {

struct window_resized_event {
  sbx::int32 width{};
  sbx::int32 height{};
}; // struct window_resized_event

struct window_moved_event {
  sbx::int32 x{};
  sbx::int32 y{};
}; // struct window_moved_event

struct framebuffer_resized_event {
  sbx::int32 width{};
  sbx::int32 height{};
}; // struct framebuffer_resized_event

struct window_minimized_event { };

struct window_maximized_event { };

struct window_restored_event { };

struct window_closed_event { };

struct monitor_connected_event { };

struct monitor_disconnected_event { };

struct key_pressed_event {
  key key;
  modifiers modifiers{};
}; // struct key_pressed_event

struct key_released_event {
  key key;
  modifiers modifiers{};
}; // struct key_released_event

struct button_pressed_event {
  button button;
  modifiers modifiers{};
}; // struct button_pressed_event

struct button_released_event {
  button button;
  modifiers modifiers{};
}; // struct button_released_event

struct mouse_moved_event {
  sbx::int32 x{};
  sbx::int32 y{};
}; // struct mouse_moved_event

struct mouse_scrolled_event {
  sbx::float32 x{};
  sbx::float32 y{};
}; // struct mouse_scrolled_event

} // namespace demo

#endif // SBX_DEMO_EVENTS_HPP_
