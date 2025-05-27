#ifndef LIBSBX_UTILITY_LOGGER_HPP_
#define LIBSBX_UTILITY_LOGGER_HPP_

#include <iostream>
#include <optional>

#include <fmt/format.h>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <libsbx/utility/target.hpp>
#include <libsbx/utility/string_literal.hpp>

namespace sbx::utility {

template<string_literal Tag>
class logger {

public:

  template<typename... Args>
  using format_string_type = spdlog::format_string_t<Args...>;

  logger() = delete;

  ~logger() = default;

  template<typename... Args>
  static auto trace(format_string_type<Args...> format, Args&&... args) -> void {
    // [NOTE] KAJ 2023-03-20 : This should make trace and debug messages be no-ops in release builds.
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance().trace(format, std::forward<Args>(args)...);
    }
  }

  template<typename Type>
  static auto trace(const Type& value) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance().trace(value);
    }
  }

  template<typename... Args>
  static auto debug(format_string_type<Args...> format, Args&&... args) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance().debug(format, std::forward<Args>(args)...);
    }
  }

  template<typename Type>
  static auto debug(const Type& value) -> void {
    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      _instance().debug(value);
    }
  }

  template<typename... Args>
  static auto info(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().info(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto info(const Type& value) -> void {
    _instance().info(value);
  }

  template<typename... Args>
  static auto warn(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().warn(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto warn(const Type& value) -> void {
    _instance().warn(value);
  }

  template<typename... Args>
  static auto error(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().error(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto error(const Type& value) -> void {
    _instance().error(value);
  }

  template<typename... Args>
  static auto critical(format_string_type<Args...> format, Args&&... args) -> void {
    _instance().critical(format, std::forward<Args>(args)...);
  }

  template<typename Type>
  static auto critical(const Type& value) -> void {
    _instance().critical(value);
  }

private:

  static auto _instance() -> spdlog::logger& {
    static auto instance = _create_logger();
    return instance;
  }

  static auto _create_logger() -> spdlog::logger {
    auto sinks = std::vector<std::shared_ptr<spdlog::sinks::sink>>{};

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("./demo/logs/sbx.log", true));

    if constexpr (utility::build_configuration_v == utility::build_configuration::debug) {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    auto logger = spdlog::logger{Tag, std::begin(sinks), std::end(sinks)};

    logger.set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] [%n] : %v");

    if constexpr (build_configuration_v == build_configuration::debug) {
      logger.set_level(spdlog::level::debug);
    } else {
      logger.set_level(spdlog::level::info);
    }

    return logger;
  }

}; // class logger

} // namespace sbx::utility

// [NOTE] KAJ 2024-01-19 : Enable formatting to underlying type for all enums
template<typename Type>
requires (std::is_enum_v<Type>)
struct fmt::formatter<Type> : public fmt::formatter<std::underlying_type_t<Type>> {

  using base_type = fmt::formatter<std::underlying_type_t<Type>>;

  template<typename FormatContext>
  auto format(const Type& value, FormatContext& context) -> decltype(auto) {
    return base_type::format(static_cast<std::underlying_type_t<Type>>(value), context);
  }

}; // struct fmt::formatter

// [NOTE] KAJ 2025-05-27 : Enable formatting for optionals
template<typename Type>
struct fmt::formatter<std::optional<Type>> : public fmt::formatter<Type> {

  using base_type = fmt::formatter<Type>;

  template<typename FormatContext>
  auto format(const std::optional<Type>& value, FormatContext& context) -> decltype(auto) {
    if (value) {
      return base_type::format(*value, context);
    }

    return fmt::format_to(context.out(), "[empty optional]");
  }

}; // struct fmt::formatter<Type>


#endif // LIBSBX_UTILITY_LOGGER_HPP_
