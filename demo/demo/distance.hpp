#ifndef DEMO_DISTANCE_HPP_
#define DEMO_DISTANCE_HPP_

#include <demo/quantity.hpp>

namespace demo {

namespace detail {
  
struct distance_dimension { };

} // namespace detail

using kilometer = quantity<detail::distance_dimension, std::float_t, std::kilo>;
using meter = quantity<detail::distance_dimension, std::float_t>;
using decimeter = quantity<detail::distance_dimension, std::float_t, std::deci>;
using centimeter = quantity<detail::distance_dimension, std::float_t, std::centi>;
using millimeter = quantity<detail::distance_dimension, std::float_t, std::milli>;

namespace literals {

inline auto operator"" _km(long double value) -> kilometer {
  return kilometer{static_cast<kilometer::value_type>(value)};
}

inline auto operator"" _km(unsigned long long value) -> kilometer {
  return kilometer{static_cast<kilometer::value_type>(value)};
}

inline auto operator"" _m(long double value) -> meter {
  return meter{static_cast<meter::value_type>(value)};
}

inline auto operator"" _m(unsigned long long value) -> meter {
  return meter{static_cast<meter::value_type>(value)};
}

inline auto operator"" _dm(long double value) -> decimeter {
  return decimeter{static_cast<decimeter::value_type>(value)};
}

inline auto operator"" _dm(unsigned long long value) -> decimeter {
  return decimeter{static_cast<decimeter::value_type>(value)};
}

inline auto operator"" _cm(long double value) -> centimeter {
  return centimeter{static_cast<centimeter::value_type>(value)};
}

inline auto operator"" _cm(unsigned long long value) -> centimeter {
  return centimeter{static_cast<centimeter::value_type>(value)};
}

inline auto operator"" _mm(long double value) -> millimeter {
  return millimeter{static_cast<millimeter::value_type>(value)};
}

inline auto operator"" _mm(unsigned long long value) -> millimeter {
  return millimeter{static_cast<millimeter::value_type>(value)};
}

} // namespace literals

} // namespace demo

#endif // DEMO_DISTANCE_HPP_
