#ifndef SBX_EVTSYS_EVENT_HPP_
#define SBX_EVTSYS_EVENT_HPP_

#include <cinttypes>

namespace sbx {

/**
 * @brief Describes the type of an event with the corresponding category flag set
 */
enum class event_type : std::uint8_t {
  KEY_PRESSED         = 0x2f,
  KEY_REPEATED        = 0x30,
  KEY_RELEASED        = 0x31,
  BUTTON_PRESSED      = 0x49,
  BUTTON_RELEASED     = 0x4a,
  CURSOR_MOVED        = 0x4b,
  CURSOR_ENTERED      = 0x4c,
  CURSOR_LEFT         = 0x4d,
  SCROLLED            = 0x4e,
  WINDOW_MOVED        = 0x80,
  WINDOW_RESIZED      = 0x81,
  WINDOW_CLOSED       = 0x82,
  WINDOW_REFRESHED    = 0x83,
  WINDOW_FOCUSED      = 0x84,
  WINDOW_UNFOCUSED    = 0x85,
  WINDOW_ICONIFIED    = 0x86,
  WINDOW_UNICONIFIED  = 0x87,
  FRAMEBUFFER_RESIZED = 0x88
}; // enum class event_type

/**
 * @brief Flag for a specific event category
 */
enum class event_category : std::uint8_t {
  KEYBOARD = 0x20,
  MOUSE    = 0x40,
  WINDOW   = 0x80
}; // enum class event_type

/**
 * @brief Bit mask for event types
 */
constexpr std::uint8_t EVENT_TYPE_MASK     = 0x1f;

/**
 * @brief Bot mask for event categories
 */
constexpr std::uint8_t EVENT_CATEGORY_MASK = 0xe0;

/**
 * @class event
 * 
 * @brief Describes as basic event
 */
class event {

public:
  event() = default;
  virtual ~event() = default;

  bool is_in_category(event_category category) const;

  virtual event_type type() const = 0;

}; // class event

} // namespace sbx

#endif // SBX_EVTSYS_EVENT_HPP_