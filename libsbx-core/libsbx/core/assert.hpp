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

template<std::convertible_to<bool> Expression>
inline auto assert_that(Expression&& expression, std::string_view message, const std::source_location& source_location = std::source_location::current()) -> void {
  if constexpr (build_configuration_v == build_configuration::debug) {
    if (!static_cast<bool>(expression)) {
      logger::error("sbx::core", "Assertion '{}' at {}:{} in '{}' failed. Terminating", message, source_location.file_name(), source_location.line(), source_location.function_name());
      std::terminate();
    }
  }
}

template<std::convertible_to<bool> Expression>
inline auto expect_that(Expression&& expression, std::string_view message, const std::source_location& source_location = std::source_location::current()) -> void {
  if constexpr (build_configuration_v == build_configuration::debug) {
    if (!static_cast<bool>(expression)) {
      logger::warn("sbx::core", "Expectation '{}' at {}:{} in '{}' failed.", message, source_location.file_name(), source_location.line(), source_location.function_name());
    }
  }
}

} // namespace sbx::core

#endif // LIBSBX_CORE_ASSERT_HPP_
