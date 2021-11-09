#ifndef SBX_CORE_LOGGER_HPP_
#define SBX_CORE_LOGGER_HPP_

#include <string_view>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sbx {

class logger {

public:

  logger() = default;

  ~logger() = default;

  template<typename... Args>
  static void trace(std::string_view message, Args&&... args) {
    _logger->trace(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void debug(std::string_view message, Args&&... args) {
    _logger->debug(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void info(std::string_view message, Args&&... args) {
    _logger->info(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void warn(std::string_view message, Args&&... args) {
    _logger->warn(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void error(std::string_view message, Args&&... args) {
    _logger->error(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void critical(std::string_view message, Args&&... args) {
    _logger->critical(message, std::forward<Args>(args)...);
  }

private:

  static void _initialize();

  static std::shared_ptr<spdlog::logger> _logger{};

  friend class engine;

}; // class logger

} // namespace sbx

#endif // SBX_CORE_LOGGER_HPP_
