#ifndef DEMO_LOGGER_HPP_
#define DEMO_LOGGER_HPP_

#include <memory>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace demo {

class logger {

  template<typename... Args>
  using format_string_type = spdlog::format_string_t<Args...>;

public:

  logger()
  : _logger{std::make_unique<spdlog::logger>("sbx", std::make_shared<spdlog::sinks::stdout_color_sink_mt>())} {
    _logger->set_pattern("[%Y-%m-%d %H:%M:%S %z] [%n] [%^%l%$] %v");
    _logger->set_level(spdlog::level::trace);
  }

  ~logger() = default;

  template<typename... Args>
  void trace(format_string_type<Args...> format, Args&&... args) {
    _logger->trace(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void debug(format_string_type<Args...> format, Args&&... args) {
    _logger->debug(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void info(format_string_type<Args...> format, Args&&... args) {
    _logger->info(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void warn(format_string_type<Args...> format, Args&&... args) {
    _logger->warn(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(format_string_type<Args...> format, Args&&... args) {
    _logger->error(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void critical(format_string_type<Args...> format, Args&&... args) {
    _logger->critical(format, std::forward<Args>(args)...);
  }

private:

  std::unique_ptr<spdlog::logger> _logger{};

}; // class logger

} // namespace demo

#endif // DEMO_LOGGER_HPP_
