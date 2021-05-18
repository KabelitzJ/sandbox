#include "event.hpp"

#include <type_traits>

namespace sbx {

bool event::is_in_category(event_category category) const {
  std::underlying_type_t<event_type> type_value = static_cast<std::underlying_type_t<event_type>>(type());
  std::underlying_type_t<event_category> category_value = static_cast<std::underlying_type_t<event_category>>(category);

  return (type_value & EVENT_CATEGORY_MASK) == category_value;
}

} // namespace sbx
