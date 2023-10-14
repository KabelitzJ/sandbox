#ifndef LIBSBX_DEVICES_EVENTS_HPP_
#define LIBSBX_DEVICES_EVENTS_HPP_

#include <cinttypes>
#include <cmath>

#include <libsbx/devices/key.hpp>
#include <libsbx/devices/mouse_button.hpp>
#include <libsbx/devices/input.hpp>

namespace sbx::devices {

/** 
 * @brief An event that is fired when a window requests to be closed 
 */
struct window_closed_event { };

struct window_moved_event {
  std::int32_t x{};
  std::int32_t y{};
};

struct window_resized_event {
  std::int32_t width{};
  std::int32_t height{};
};

struct framebuffer_resized_event {
  std::int32_t width{};
  std::int32_t height{};
};

struct key_event {
  key key;
  input_action action;
  input_mod mods;
}; // struct key_event

struct mouse_button_event {
  mouse_button button;
  input_action action;
  input_mod mods;
}; // struct mouse_button_event

struct mouse_moved_event {
  std::float_t x;
  std::float_t y;
}; // struct mouse_moved_event

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_EVENTS_HPP_
