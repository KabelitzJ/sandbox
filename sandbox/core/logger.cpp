#include "logger.hpp"

namespace sbx {

void logger::_initialize() {
  auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  sink->set_pattern("%^%Y-%m-%d %H:%M:%S:%e [%=8l] - %v%$");

  _logger = std::make_shared<spdlog::logger>("core_logger", sink);

  spdlog::register_logger(_logger);

  _logger->set_level(spdlog::level::trace);
  _logger->flush_on(spdlog::level::trace);
}

} // namespace sbx
