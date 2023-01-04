#ifndef LIBSBX_DEVICES_EVENTS_HPP_
#define LIBSBX_DEVICES_EVENTS_HPP_

#include <cinttypes>

namespace sbx::devices {

struct window_closed_event { };

struct window_moved_event {
  std::int32_t x{};
  std::int32_t y{};
};

struct window_resized_event {
  std::int32_t width{};
  std::int32_t height{};
};

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_EVENTS_HPP_
