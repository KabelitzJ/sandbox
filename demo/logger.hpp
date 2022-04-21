#ifndef DEMO_LOGGER_HPP_
#define DEMO_LOGGER_HPP_

#include <memory>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace demo {

class logger {

public:

  logger()
  : _logger{nullptr} {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("%v");

    _logger = std::make_unique<spdlog::logger>("sbx", console_sink);
  }

private:

  std::unique_ptr<spdlog::logger> _logger{};

}; // class logger

} // namespace demo

#endif // DEMO_LOGGER_HPP_
