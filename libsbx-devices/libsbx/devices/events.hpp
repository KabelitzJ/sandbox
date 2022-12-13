#ifndef LIBSBX_DEVICES_EVENTS_HPP_
#define LIBSBX_DEVICES_EVENTS_HPP_

#include <cstdint>

#include <GLFW/glfw3.h>

#include <libsbx/devices/key.hpp>
#include <libsbx/devices/modifiers.hpp>

namespace sbx::devices {

struct key_pressed_event {
  key key;
  modifiers modifiers;
}; // struct key_pressed_event

struct key_released_event {
  key key;
  modifiers modifiers;
}; // struct key_released_event

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_EVENTS_HPP_
