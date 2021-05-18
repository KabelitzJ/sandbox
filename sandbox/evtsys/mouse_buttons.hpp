#ifndef SBX_EVTSYS_MOUSE_BUTTONS_HPP_
#define SBX_EVTSYS_MOUSE_BUTTONS_HPP_

#include <cinttypes>

namespace sbx {

/**
 * @brief Mouse buttos as defined by the glfw specification
 */
enum class mouse_button : std::uint8_t {
  MOUSE_BUTTON_1      = 0x00,
  MOUSE_BUTTON_2      = 0x01,
  MOUSE_BUTTON_3      = 0x02,
  MOUSE_BUTTON_4      = 0x03,
  MOUSE_BUTTON_5      = 0x04,
  MOUSE_BUTTON_6      = 0x05,
  MOUSE_BUTTON_7      = 0x06,
  MOUSE_BUTTON_8      = 0x07,
  MOUSE_BUTTON_LEFT   = MOUSE_BUTTON_1,
  MOUSE_BUTTON_RIGHT  = MOUSE_BUTTON_2,
  MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_3
}; // enum class mouse_button

} // namespace sbx

#endif // SBX_EVTSYS_MOUSE_BUTTONS_HPP_
