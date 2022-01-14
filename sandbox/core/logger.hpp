#ifndef SBX_CORE_LOGGER_HPP_
#define SBX_CORE_LOGGER_HPP_

#include <string_view>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sbx {

class logger {

public:

  logger(const logger&) = delete;

  logger(logger&&) = delete;

  ~logger() = default;

  logger& operator=(const logger&) = delete;
  
  logger& operator=(logger&&) = delete;

  template<typename... Args>
  static void trace(std::string_view message, Args&&... args) {
    _get_initialized_logger()->trace(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void debug(std::string_view message, Args&&... args) {
    _get_initialized_logger()->debug(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void info(std::string_view message, Args&&... args) {
    _get_initialized_logger()->info(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void warn(std::string_view message, Args&&... args) {
    _get_initialized_logger()->warn(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void error(std::string_view message, Args&&... args) {
    _get_initialized_logger()->error(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  static void critical(std::string_view message, Args&&... args) {
    _get_initialized_logger()->critical(message, std::forward<Args>(args)...);
  }

private:

  logger() = default;

  static void _initialize();
  static std::shared_ptr<spdlog::logger>& _get_initialized_logger();

  inline static bool _is_initialized{false};
  inline static std::shared_ptr<spdlog::logger> _logger{};

}; // class logger

} // namespace sbx

#endif // SBX_CORE_LOGGER_HPP_
