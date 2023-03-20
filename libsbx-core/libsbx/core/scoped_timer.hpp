#ifndef LIBSBX_CORE_SCOPED_TIMER_HPP_
#define LIBSBX_CORE_SCOPED_TIMER_HPP_

#include <chrono>

#include <libsbx/core/delegate.hpp>
#include <libsbx/core/target.hpp>
#include <libsbx/core/macros.hpp>

#include <libsbx/units/time.hpp>

namespace sbx::core {

class scoped_timer {

public:

  template<typename Callback>
  requires (std::is_invocable_v<Callback, const units::second&>)
  scoped_timer(Callback&& callback) noexcept
  : _callback{std::forward<Callback>(callback)},
    _start{std::chrono::high_resolution_clock::now()} { }

  ~scoped_timer() noexcept {
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - _start);

    _callback(units::second{duration.count()});
  } 

private:

  delegate<void(const units::second&)> _callback;
  std::chrono::high_resolution_clock::time_point _start;

}; // class scoped_timer

#if defined(SBX_DEBUG)
#define SBX_DEBUG_SCOPED_TIMER(callback) auto SBX_UNIQUE_NAME(_debug_timer_) = sbx::core::scoped_timer{callback}
#else
#define SBX_DEBUG_SCOPED_TIMER(callback)
#endif // defined(SBX_DEBUG)

} // namespace sbx::core

#endif // LIBSBX_CORE_SCOPED_TIMER_HPP_
