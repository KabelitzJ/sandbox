#ifndef DEMO_VELOCITY_HPP_
#define DEMO_VELOCITY_HPP_

#include <demo/distance.hpp>
#include <demo/time.hpp>

namespace demo {

namespace detail {

struct velocity : distance, time { };

} // namespace detail

using velocity = quantity<detail::velocity, std::float_t>;

auto operator/(const meter& lhs, const second& rhs) -> velocity {
  return velocity{lhs.value() / rhs.value()};
}

} // namespace demo

#endif // DEMO_VELOCITY_HPP_
