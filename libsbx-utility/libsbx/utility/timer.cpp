#include <libsbx/utility/timer.hpp>

namespace sbx::utility {

timer::timer()
: _start{std::chrono::high_resolution_clock::now()} { }

auto timer::elapsed() const noexcept -> units::second {
  const auto now = std::chrono::high_resolution_clock::now();
  return units::second{std::chrono::duration_cast<std::chrono::duration<std::float_t>>(now - _start).count()};
}

} // namespace sbx::utility
