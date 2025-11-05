#ifndef LIBSBX_SCRIPTING_INTEROP_HPP_
#define LIBSBX_SCRIPTING_INTEROP_HPP_

#include <libsbx/utility/enum.hpp>

#include <libsbx/scripting/managed/string.hpp>

namespace sbx::scripting {

struct interop {

  enum class log_level : std::int32_t {
    trace = utility::bit_v<0>,
    debug = utility::bit_v<1>,
    info = utility::bit_v<2>,
    warn = utility::bit_v<3>,
    error = utility::bit_v<4>,
    critical = utility::bit_v<5>
  }; // enum class log_level

  static auto log_log_message(log_level level, managed::string message) -> void;

}; // class interop

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_INTEROP_HPP_