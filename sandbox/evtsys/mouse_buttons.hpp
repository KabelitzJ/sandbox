#ifndef SBX_EVTSYS_MOUSE_BUTTONS_HPP_
#define SBX_EVTSYS_MOUSE_BUTTONS_HPP_

#include <cinttypes>

namespace sbx {

/**
 * @brief Mouse buttos as defined by the glfw specification
 */
enum class mouse_button : std::uint8_t {
  ONE    = 0x00,
  TWO    = 0x01,
  THREE  = 0x02,
  FOUR   = 0x03,
  FIVE   = 0x04,
  SIX    = 0x05,
  SEVEN  = 0x06,
  EIGHT  = 0x07,
  LEFT   = ONE,
  RIGHT  = TWO,
  MIDDLE = THREE
}; // enum class mouse_button

} // namespace sbx

#endif // SBX_EVTSYS_MOUSE_BUTTONS_HPP_
