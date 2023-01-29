#ifndef LIBSBX_CORE_ASSERT_HPP_
#define LIBSBX_CORE_ASSERT_HPP_

#include <source_location>
#include <concepts>
#include <string_view>
#include <cstdlib>

#include <fmt/format.h>

#include <libsbx/core/target.hpp>
#include <libsbx/core/logger.hpp>

namespace sbx::core {

class assertion_failure : public std::runtime_error {

public:

  assertion_failure(const std::string& message)
  : std::runtime_error{message} { } 

  ~assertion_failure() noexcept override = default;

}; // class assertion_failure

template<std::convertible_to<bool> Expression>
inline auto assert_that(Expression&& expression, std::string_view message, const std::source_location& source_location = std::source_location::current()) -> void {
  if constexpr (build_configuration_v == build_configuration::debug) {
    if (!static_cast<bool>(expression)) {
      const auto formatted_message = fmt::format("Assertion '{}' at {}:{} in '{}' failed", message, source_location.file_name(), source_location.line(), source_location.function_name());
      if constexpr (has_exceptions_v) {
        throw assertion_failure{formatted_message};
      } else {
        logger::error("{}", formatted_message);
        std::exit(EXIT_FAILURE);
      }
    }
  }
}

} // namespace sbx::core

#endif // LIBSBX_CORE_ASSERT_HPP_
