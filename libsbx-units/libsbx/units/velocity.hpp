#ifndef LIBSBX_UNITS_VELOCITY_HPP_
#define LIBSBX_UNITS_VELOCITY_HPP_

#include <libsbx/units/distance.hpp>
#include <libsbx/units/time.hpp>

namespace sbx::units {

namespace detail {

struct velocity_tag : distance_tag, time_tag { };

} // namespace detail

using velocity = quantity<detail::velocity_tag, std::float_t>;

auto operator/(const meter& lhs, const second& rhs) -> velocity {
  return velocity{lhs.value() / rhs.value()};
}

} // namespace sbx::units

#endif // LIBSBX_UNITS_VELOCITY_HPP_
