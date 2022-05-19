#ifndef SBX_DEMO_EVENTS_HPP_
#define SBX_DEMO_EVENTS_HPP_

#include <types/primitives.hpp>

namespace demo {

struct window_resized_event {
  sbx::int32 width{};
  sbx::int32 height{};
}; // struct window_resized_event

struct window_moved_event {
  sbx::int32 x{};
  sbx::int32 y{};
}; // struct window_moved_event

struct mouse_moved_event {
  sbx::int32 x{};
  sbx::int32 y{};
}; // struct mouse_moved_event

struct window_minimized_event { };

struct window_maximized_event { };

struct window_restored_event { };

struct window_closed_event { };

struct monitor_connected_event { };

struct monitor_disconnected_event { };

struct key_pressed_event {
  sbx::int32 key{};
  sbx::int32 mods{};
}; // struct key_pressed_event

struct key_released_event {
  sbx::int32 key{};
  sbx::int32 mods{};
}; // struct key_released_event

} // namespace demo

#endif // SBX_DEMO_EVENTS_HPP_
