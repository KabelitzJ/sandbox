#include <libsbx/devices/key.hpp>

#include <GLFW/glfw3.h>

namespace sbx::devices {

const key key::unknown{GLFW_KEY_UNKNOWN, 0};
const key key::space{GLFW_KEY_SPACE, 0};
const key key::apostrophe{GLFW_KEY_APOSTROPHE, 0};
const key key::comma{GLFW_KEY_COMMA, 0};
const key key::minus{GLFW_KEY_MINUS, 0};
const key key::period{GLFW_KEY_PERIOD, 0};
const key key::slash{GLFW_KEY_SLASH, 0};
const key key::zero{GLFW_KEY_0, 0};
const key key::one{GLFW_KEY_1, 0};
const key key::two{GLFW_KEY_2, 0};
const key key::three{GLFW_KEY_3, 0};
const key key::four{GLFW_KEY_4, 0};
const key key::five{GLFW_KEY_5, 0};
const key key::six{GLFW_KEY_6, 0};
const key key::seven{GLFW_KEY_7, 0};
const key key::eight{GLFW_KEY_8, 0};
const key key::nine{GLFW_KEY_9, 0};
const key key::semicolon{GLFW_KEY_SEMICOLON, 0};
const key key::equal{GLFW_KEY_EQUAL, 0};
const key key::a{GLFW_KEY_A, 0};
const key key::b{GLFW_KEY_B, 0};
const key key::c{GLFW_KEY_C, 0};
const key key::d{GLFW_KEY_D, 0};
const key key::e{GLFW_KEY_E, 0};
const key key::f{GLFW_KEY_F, 0};
const key key::g{GLFW_KEY_G, 0};
const key key::h{GLFW_KEY_H, 0};
const key key::i{GLFW_KEY_I, 0};
const key key::j{GLFW_KEY_J, 0};
const key key::k{GLFW_KEY_K, 0};
const key key::l{GLFW_KEY_L, 0};
const key key::m{GLFW_KEY_M, 0};
const key key::n{GLFW_KEY_N, 0};
const key key::o{GLFW_KEY_O, 0};
const key key::p{GLFW_KEY_P, 0};
const key key::q{GLFW_KEY_Q, 0};
const key key::r{GLFW_KEY_R, 0};
const key key::s{GLFW_KEY_S, 0};
const key key::t{GLFW_KEY_T, 0};
const key key::u{GLFW_KEY_U, 0};
const key key::v{GLFW_KEY_V, 0};
const key key::w{GLFW_KEY_W, 0};
const key key::x{GLFW_KEY_X, 0};
const key key::y{GLFW_KEY_Y, 0};
const key key::z{GLFW_KEY_Z, 0};
const key key::left_bracket{GLFW_KEY_LEFT_BRACKET, 0};
const key key::backslash{GLFW_KEY_BACKSLASH, 0};
const key key::right_bracket{GLFW_KEY_RIGHT_BRACKET, 0};
const key key::grave_accent{GLFW_KEY_GRAVE_ACCENT, 0};
const key key::world_1{GLFW_KEY_WORLD_1, 0};
const key key::world_2{GLFW_KEY_WORLD_2, 0};
const key key::escape{GLFW_KEY_ESCAPE, 0};
const key key::enter{GLFW_KEY_ENTER, 0};
const key key::tab{GLFW_KEY_TAB, 0};
const key key::backspace{GLFW_KEY_BACKSPACE, 0};
const key key::insert{GLFW_KEY_INSERT, 0};
const key key::del{GLFW_KEY_DELETE, 0};
const key key::right{GLFW_KEY_RIGHT, 0};
const key key::left{GLFW_KEY_LEFT, 0};
const key key::down{GLFW_KEY_DOWN, 0};
const key key::up{GLFW_KEY_UP, 0};
const key key::page_up{GLFW_KEY_PAGE_UP, 0};
const key key::page_down{GLFW_KEY_PAGE_DOWN, 0};
const key key::home{GLFW_KEY_HOME, 0};
const key key::end{GLFW_KEY_END, 0};
const key key::caps_lock{GLFW_KEY_CAPS_LOCK, 0};
const key key::scroll_lock{GLFW_KEY_SCROLL_LOCK, 0};
const key key::num_lock{GLFW_KEY_NUM_LOCK, 0};
const key key::print_screen{GLFW_KEY_PRINT_SCREEN, 0};
const key key::pause{GLFW_KEY_PAUSE, 0};
const key key::f1{GLFW_KEY_F1, 0};
const key key::f2{GLFW_KEY_F2, 0};
const key key::f3{GLFW_KEY_F3, 0};
const key key::f4{GLFW_KEY_F4, 0};
const key key::f5{GLFW_KEY_F5, 0};
const key key::f6{GLFW_KEY_F6, 0};
const key key::f7{GLFW_KEY_F7, 0};
const key key::f8{GLFW_KEY_F8, 0};
const key key::f9{GLFW_KEY_F9, 0};
const key key::f10{GLFW_KEY_F10, 0};
const key key::f11{GLFW_KEY_F11, 0};
const key key::f12{GLFW_KEY_F12, 0};
const key key::f13{GLFW_KEY_F13, 0};
const key key::f14{GLFW_KEY_F14, 0};
const key key::f15{GLFW_KEY_F15, 0};
const key key::f16{GLFW_KEY_F16, 0};
const key key::f17{GLFW_KEY_F17, 0};
const key key::f18{GLFW_KEY_F18, 0};
const key key::f19{GLFW_KEY_F19, 0};
const key key::f20{GLFW_KEY_F20, 0};
const key key::f21{GLFW_KEY_F21, 0};
const key key::f22{GLFW_KEY_F22, 0};
const key key::f23{GLFW_KEY_F23, 0};
const key key::f24{GLFW_KEY_F24, 0};
const key key::f25{GLFW_KEY_F25, 0};
const key key::kp_0{GLFW_KEY_KP_0, 0};
const key key::kp_1{GLFW_KEY_KP_1, 0};
const key key::kp_2{GLFW_KEY_KP_2, 0};
const key key::kp_3{GLFW_KEY_KP_3, 0};
const key key::kp_4{GLFW_KEY_KP_4, 0};
const key key::kp_5{GLFW_KEY_KP_5, 0};
const key key::kp_6{GLFW_KEY_KP_6, 0};
const key key::kp_7{GLFW_KEY_KP_7, 0};
const key key::kp_8{GLFW_KEY_KP_8, 0};
const key key::kp_9{GLFW_KEY_KP_9, 0};
const key key::kp_decimal{GLFW_KEY_KP_DECIMAL, 0};
const key key::kp_divide{GLFW_KEY_KP_DIVIDE, 0};
const key key::kp_multiply{GLFW_KEY_KP_MULTIPLY, 0};
const key key::kp_subtract{GLFW_KEY_KP_SUBTRACT, 0};
const key key::kp_add{GLFW_KEY_KP_ADD, 0};
const key key::kp_enter{GLFW_KEY_KP_ENTER, 0};
const key key::kp_equal{GLFW_KEY_KP_EQUAL, 0};
const key key::left_shift{GLFW_KEY_LEFT_SHIFT, 0};
const key key::left_control{GLFW_KEY_LEFT_CONTROL, 0};
const key key::left_alt{GLFW_KEY_LEFT_ALT, 0};
const key key::left_super{GLFW_KEY_LEFT_SUPER, 0};
const key key::right_shift{GLFW_KEY_RIGHT_SHIFT, 0};
const key key::right_control{GLFW_KEY_RIGHT_CONTROL, 0};
const key key::right_alt{GLFW_KEY_RIGHT_ALT, 0};
const key key::right_super{GLFW_KEY_RIGHT_SUPER, 0};
const key key::menu{GLFW_KEY_MENU, 0};

bool key::operator==(const key& other) const noexcept {
  return _value == other._value;
}

key::operator std::int32_t() const noexcept {
  return _value;
}

key::key(std::int32_t value, std::int32_t scancode)
: _value{value},
  _scancode{scancode} { }

std::ostream& operator<<(std::ostream& output_stream, const key& key) {
  return output_stream << glfwGetKeyName(key._value, key._scancode);
}

} // namespace sbx::devices
