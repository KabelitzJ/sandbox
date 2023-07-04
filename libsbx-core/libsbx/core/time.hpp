#ifndef LIBSBX_CORE_TIME_HPP_
#define LIBSBX_CORE_TIME_HPP_

#include <libsbx/units/time.hpp>

namespace sbx::core {

class time {

  friend class engine;

public:

  time() = delete;

  static auto delta_time() -> units::second {
    return _delta_time;
  }

private:

  static units::second _delta_time;

}; // class time

} // namespace sbx::core

#endif // LIBSBX_CORE_TIME_HPP_
