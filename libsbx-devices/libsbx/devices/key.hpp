#ifndef LIBSBX_DEVICES_KEY_HPP_
#define LIBSBX_DEVICES_KEY_HPP_

#include <type_traits>
#include <functional>
#include <cinttypes>
#include <iostream>
#include <string_view>

#include <fmt/format.h>

namespace sbx::devices {

class key {

  friend class window;

public:

  static const key unknown;
  static const key space;
  static const key apostrophe;
  static const key comma;
  static const key minus;
  static const key period;
  static const key slash;
  static const key zero;
  static const key one;
  static const key two;
  static const key three;
  static const key four;
  static const key five;
  static const key six;
  static const key seven;
  static const key eight;
  static const key nine;
  static const key semicolon;
  static const key equal;
  static const key a;
  static const key b;
  static const key c;
  static const key d;
  static const key e;
  static const key f;
  static const key g;
  static const key h;
  static const key i;
  static const key j;
  static const key k;
  static const key l;
  static const key m;
  static const key n;
  static const key o;
  static const key p;
  static const key q;
  static const key r;
  static const key s;
  static const key t;
  static const key u;
  static const key v;
  static const key w;
  static const key x;
  static const key y;
  static const key z;
  static const key left_bracket;
  static const key backslash;
  static const key right_bracket;
  static const key grave_accent;
  static const key world_1;
  static const key world_2;
  static const key escape;
  static const key enter;
  static const key tab;
  static const key backspace;
  static const key insert;
  static const key del;
  static const key right;
  static const key left;
  static const key down;
  static const key up;
  static const key page_up;
  static const key page_down;
  static const key home;
  static const key end;
  static const key caps_lock;
  static const key scroll_lock;
  static const key num_lock;
  static const key print_screen;
  static const key pause;
  static const key f1;
  static const key f2;
  static const key f3;
  static const key f4;
  static const key f5;
  static const key f6;
  static const key f7;
  static const key f8;
  static const key f9;
  static const key f10;
  static const key f11;
  static const key f12;
  static const key f13;
  static const key f14;
  static const key f15;
  static const key f16;
  static const key f17;
  static const key f18;
  static const key f19;
  static const key f20;
  static const key f21;
  static const key f22;
  static const key f23;
  static const key f24;
  static const key f25;
  static const key kp_0;
  static const key kp_1;
  static const key kp_2;
  static const key kp_3;
  static const key kp_4;
  static const key kp_5;
  static const key kp_6;
  static const key kp_7;
  static const key kp_8;
  static const key kp_9;
  static const key kp_decimal;
  static const key kp_divide;
  static const key kp_multiply;
  static const key kp_subtract;
  static const key kp_add;
  static const key kp_enter;
  static const key kp_equal;
  static const key left_shift;
  static const key left_control;
  static const key left_alt;
  static const key left_super;
  static const key right_shift;
  static const key right_control;
  static const key right_alt;
  static const key right_super;
  static const key menu;

  ~key() = default;

  bool operator==(const key& other) const noexcept;

  operator std::string_view() const noexcept;

private:

  key(std::int32_t value, std::int32_t code);

  std::int32_t _value{};
  std::int32_t _code{};

}; // class key

} // namespace sbx::devices

#endif // LIBSBX_DEVICES_KEY_HPP_
