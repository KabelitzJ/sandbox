#ifndef SBX_CORE_LOGGER_HPP_
#define SBX_CORE_LOGGER_HPP_

#include <string_view>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sbx {

class logger {

public:

  logger() {
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("%^%Y-%m-%d %H:%M:%S:%e [%=8l] - %v%$");
  };

  ~logger() = default;

  template<typename... Args>
  void trace(std::string_view message, Args&&... args) {
    spdlog::trace(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void debug(std::string_view message, Args&&... args) {
    spdlog::debug(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void info(std::string_view message, Args&&... args) {
    spdlog::info(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void warn(std::string_view message, Args&&... args) {
    spdlog::warn(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(std::string_view message, Args&&... args) {
    spdlog::error(message, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void critical(std::string_view message, Args&&... args) {
    spdlog::critical(message, std::forward<Args>(args)...);
  }

}; // class logger

} // namespace sbx

#endif // SBX_CORE_LOGGER_HPP_
