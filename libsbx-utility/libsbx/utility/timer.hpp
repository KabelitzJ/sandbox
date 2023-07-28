#ifndef LIBSBX_UTILITY_TIMER_HPP_
#define LIBSBX_UTILITY_TIMER_HPP_

#include <chrono>
#include <cmath>

#include <libsbx/units/time.hpp>

namespace sbx::utility {

class timer {

public:

  timer();

  ~timer() = default;

  auto elapsed() const noexcept -> units::second;

private:

  std::chrono::time_point<std::chrono::high_resolution_clock> _start{};

}; // class timer

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_TIMER_HPP_
