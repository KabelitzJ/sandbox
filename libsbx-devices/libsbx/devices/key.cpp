#include <libsbx/devices/key.hpp>

#include <GLFW/glfw3.h>

namespace sbx::devices {

const key key::unknown{GLFW_KEY_UNKNOWN};
const key key::space{GLFW_KEY_SPACE};
const key key::apostrophe{GLFW_KEY_APOSTROPHE};
const key key::comma{GLFW_KEY_COMMA};
const key key::minus{GLFW_KEY_MINUS};
const key key::period{GLFW_KEY_PERIOD};
const key key::slash{GLFW_KEY_SLASH};
const key key::zero{GLFW_KEY_0};
const key key::one{GLFW_KEY_1};
const key key::two{GLFW_KEY_2};
const key key::three{GLFW_KEY_3};
const key key::four{GLFW_KEY_4};
const key key::five{GLFW_KEY_5};
const key key::six{GLFW_KEY_6};
const key key::seven{GLFW_KEY_7};
const key key::eight{GLFW_KEY_8};
const key key::nine{GLFW_KEY_9};
const key key::semicolon{GLFW_KEY_SEMICOLON};
const key key::equal{GLFW_KEY_EQUAL};
const key key::a{GLFW_KEY_A};
const key key::b{GLFW_KEY_B};
const key key::c{GLFW_KEY_C};
const key key::d{GLFW_KEY_D};
const key key::e{GLFW_KEY_E};
const key key::f{GLFW_KEY_F};
const key key::g{GLFW_KEY_G};
const key key::h{GLFW_KEY_H};
const key key::i{GLFW_KEY_I};
const key key::j{GLFW_KEY_J};
const key key::k{GLFW_KEY_K};
const key key::l{GLFW_KEY_L};
const key key::m{GLFW_KEY_M};
const key key::n{GLFW_KEY_N};
const key key::o{GLFW_KEY_O};
const key key::p{GLFW_KEY_P};
const key key::q{GLFW_KEY_Q};
const key key::r{GLFW_KEY_R};
const key key::s{GLFW_KEY_S};
const key key::t{GLFW_KEY_T};
const key key::u{GLFW_KEY_U};
const key key::v{GLFW_KEY_V};
const key key::w{GLFW_KEY_W};
const key key::x{GLFW_KEY_X};
const key key::y{GLFW_KEY_Y};
const key key::z{GLFW_KEY_Z};
const key key::left_bracket{GLFW_KEY_LEFT_BRACKET};
const key key::backslash{GLFW_KEY_BACKSLASH};
const key key::right_bracket{GLFW_KEY_RIGHT_BRACKET};
const key key::grave_accent{GLFW_KEY_GRAVE_ACCENT};
const key key::world_1{GLFW_KEY_WORLD_1};
const key key::world_2{GLFW_KEY_WORLD_2};
const key key::escape{GLFW_KEY_ESCAPE};
const key key::enter{GLFW_KEY_ENTER};
const key key::tab{GLFW_KEY_TAB};
const key key::backspace{GLFW_KEY_BACKSPACE};
const key key::insert{GLFW_KEY_INSERT};
const key key::del{GLFW_KEY_DELETE};
const key key::right{GLFW_KEY_RIGHT};
const key key::left{GLFW_KEY_LEFT};
const key key::down{GLFW_KEY_DOWN};
const key key::up{GLFW_KEY_UP};
const key key::page_up{GLFW_KEY_PAGE_UP};
const key key::page_down{GLFW_KEY_PAGE_DOWN};
const key key::home{GLFW_KEY_HOME};
const key key::end{GLFW_KEY_END};
const key key::caps_lock{GLFW_KEY_CAPS_LOCK};
const key key::scroll_lock{GLFW_KEY_SCROLL_LOCK};
const key key::num_lock{GLFW_KEY_NUM_LOCK};
const key key::print_screen{GLFW_KEY_PRINT_SCREEN};
const key key::pause{GLFW_KEY_PAUSE};
const key key::f1{GLFW_KEY_F1};
const key key::f2{GLFW_KEY_F2};
const key key::f3{GLFW_KEY_F3};
const key key::f4{GLFW_KEY_F4};
const key key::f5{GLFW_KEY_F5};
const key key::f6{GLFW_KEY_F6};
const key key::f7{GLFW_KEY_F7};
const key key::f8{GLFW_KEY_F8};
const key key::f9{GLFW_KEY_F9};
const key key::f10{GLFW_KEY_F10};
const key key::f11{GLFW_KEY_F11};
const key key::f12{GLFW_KEY_F12};
const key key::f13{GLFW_KEY_F13};
const key key::f14{GLFW_KEY_F14};
const key key::f15{GLFW_KEY_F15};
const key key::f16{GLFW_KEY_F16};
const key key::f17{GLFW_KEY_F17};
const key key::f18{GLFW_KEY_F18};
const key key::f19{GLFW_KEY_F19};
const key key::f20{GLFW_KEY_F20};
const key key::f21{GLFW_KEY_F21};
const key key::f22{GLFW_KEY_F22};
const key key::f23{GLFW_KEY_F23};
const key key::f24{GLFW_KEY_F24};
const key key::f25{GLFW_KEY_F25};
const key key::kp_0{GLFW_KEY_KP_0};
const key key::kp_1{GLFW_KEY_KP_1};
const key key::kp_2{GLFW_KEY_KP_2};
const key key::kp_3{GLFW_KEY_KP_3};
const key key::kp_4{GLFW_KEY_KP_4};
const key key::kp_5{GLFW_KEY_KP_5};
const key key::kp_6{GLFW_KEY_KP_6};
const key key::kp_7{GLFW_KEY_KP_7};
const key key::kp_8{GLFW_KEY_KP_8};
const key key::kp_9{GLFW_KEY_KP_9};
const key key::kp_decimal{GLFW_KEY_KP_DECIMAL};
const key key::kp_divide{GLFW_KEY_KP_DIVIDE};
const key key::kp_multiply{GLFW_KEY_KP_MULTIPLY};
const key key::kp_subtract{GLFW_KEY_KP_SUBTRACT};
const key key::kp_add{GLFW_KEY_KP_ADD};
const key key::kp_enter{GLFW_KEY_KP_ENTER};
const key key::kp_equal{GLFW_KEY_KP_EQUAL};
const key key::left_shift{GLFW_KEY_LEFT_SHIFT};
const key key::left_control{GLFW_KEY_LEFT_CONTROL};
const key key::left_alt{GLFW_KEY_LEFT_ALT};
const key key::left_super{GLFW_KEY_LEFT_SUPER};
const key key::right_shift{GLFW_KEY_RIGHT_SHIFT};
const key key::right_control{GLFW_KEY_RIGHT_CONTROL};
const key key::right_alt{GLFW_KEY_RIGHT_ALT};
const key key::right_super{GLFW_KEY_RIGHT_SUPER};
const key key::menu{GLFW_KEY_MENU};

bool key::operator==(const key& other) const noexcept {
  return _value == other._value;
}

key::operator std::int32_t() const noexcept {
  return _value;
}

key::key(std::int32_t value)
: _value{value} { }



} // namespace sbx::devices
