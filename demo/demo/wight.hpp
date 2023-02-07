#ifndef DEMO_WEIGHT_HPP_
#define DEMO_WEIGHT_HPP_

#include <demo/quantity.hpp>

namespace demo {

namespace detail {

struct weight_dimension { };

} // namespace detail

using kilogram = quantity<detail::weight_dimension, std::float_t, std::kilo>;
using gram = quantity<detail::weight_dimension, std::float_t>;
using milligram = quantity<detail::weight_dimension, std::float_t, std::milli>;

namespace literals {

auto operator"" _kg(long double value) -> kilogram {
  return kilogram{static_cast<kilogram::value_type>(value)};
}

auto operator"" _kg(unsigned long long value) -> kilogram {
  return kilogram{static_cast<kilogram::value_type>(value)};
}

auto operator"" _g(long double value) -> gram {
  return gram{static_cast<gram::value_type>(value)};
}

auto operator"" _g(unsigned long long value) -> gram {
  return gram{static_cast<gram::value_type>(value)};
}

auto operator"" _mg(long double value) -> milligram {
  return milligram{static_cast<milligram::value_type>(value)};
}

auto operator"" _mg(unsigned long long value) -> milligram {
  return milligram{static_cast<milligram::value_type>(value)};
}

} // namespace literals

} // namespace demo

#endif // DEMO_WEIGHT_HPP_
