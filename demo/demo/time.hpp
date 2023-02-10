#ifndef DEMO_TIME_HPP_
#define DEMO_TIME_HPP_

#include <demo/quantity.hpp>

namespace demo {

namespace detail {

struct time { };

} // namespace detail

using year = quantity<detail::time, std::float_t, std::ratio<31556952>>;
using month = quantity<detail::time, std::float_t, std::ratio<2629746>>;
using week = quantity<detail::time, std::float_t, std::ratio<604800>>;
using day = quantity<detail::time, std::float_t, std::ratio<86400>>;
using hour = quantity<detail::time, std::float_t, std::ratio<3600>>;
using minute = quantity<detail::time, std::float_t, std::ratio<60>>;
using second = quantity<detail::time, std::float_t>;
using millisecond = quantity<detail::time, std::float_t, std::milli>;
using microsecond = quantity<detail::time, std::float_t, std::micro>;
using nanosecond = quantity<detail::time, std::float_t, std::nano>;

namespace literals {

auto operator"" _y(long double value) -> year {
  return year{static_cast<year::value_type>(value)};
}

auto operator"" _y(unsigned long long value) -> year {
  return year{static_cast<year::value_type>(value)};
}

auto operator"" _mo(long double value) -> month {
  return month{static_cast<month::value_type>(value)};
}

auto operator"" _mo(unsigned long long value) -> month {
  return month{static_cast<month::value_type>(value)};
}

auto operator"" _w(long double value) -> week {
  return week{static_cast<week::value_type>(value)};
}

auto operator"" _w(unsigned long long value) -> week {
  return week{static_cast<week::value_type>(value)};
}

auto operator"" _d(long double value) -> day {
  return day{static_cast<day::value_type>(value)};
}

auto operator"" _d(unsigned long long value) -> day {
  return day{static_cast<day::value_type>(value)};
}

auto operator"" _h(long double value) -> hour {
  return hour{static_cast<hour::value_type>(value)};
}

auto operator"" _h(unsigned long long value) -> hour {
  return hour{static_cast<hour::value_type>(value)};
}

auto operator"" _min(long double value) -> minute {
  return minute{static_cast<minute::value_type>(value)};
}

auto operator"" _min(unsigned long long value) -> minute {
  return minute{static_cast<minute::value_type>(value)};
}

auto operator"" _s(long double value) -> second {
  return second{static_cast<second::value_type>(value)};
}

auto operator"" _s(unsigned long long value) -> second {
  return second{static_cast<second::value_type>(value)};
}

auto operator"" _ms(long double value) -> millisecond {
  return millisecond{static_cast<millisecond::value_type>(value)};
}

auto operator"" _ms(unsigned long long value) -> millisecond {
  return millisecond{static_cast<millisecond::value_type>(value)};
}

auto operator"" _us(long double value) -> microsecond {
  return microsecond{static_cast<microsecond::value_type>(value)};
}

auto operator"" _us(unsigned long long value) -> microsecond {
  return microsecond{static_cast<microsecond::value_type>(value)};
}

auto operator"" _ns(long double value) -> nanosecond {
  return nanosecond{static_cast<nanosecond::value_type>(value)};
}

auto operator"" _ns(unsigned long long value) -> nanosecond {
  return nanosecond{static_cast<nanosecond::value_type>(value)};
}

};

} // namespace demo

#endif // DEMO_TIME_HPP_
