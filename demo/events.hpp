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

} // namespace demo

#endif // SBX_DEMO_EVENTS_HPP_
