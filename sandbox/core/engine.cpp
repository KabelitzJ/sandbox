#include "engine.hpp"

#include <chrono>

#include <types/primitives.hpp>

#include "logger.hpp"

namespace sbx {

engine::engine()
: _registry{std::make_unique<registry>()},
  _scheduler{std::make_unique<scheduler>()},
  _event_queue{std::make_unique<event_queue>()},
  _logger{std::make_unique<logger>()},
  _modules{} { }

engine::~engine() {
  
}

void engine::initialize() {
  module::_registry = _registry.get();
  module::_scheduler = _scheduler.get();
  module::_event_queue = _event_queue.get();
  module::_logger = _logger.get();

  _logger->info("initializing modules...");

  for (auto& [id, module] : _modules) {
    module->initialize();
  }

  _logger->info("finished initializing modules");
}

void engine::start() {
  _logger->info("starting main loop");

  using clock = std::chrono::steady_clock;
  using duration = std::chrono::duration<time>;

  auto last_time = clock::now();

  while (!_scheduler->is_empty()) {
    const auto now = clock::now();

    const auto delta_time = std::chrono::duration_cast<duration>(now - last_time).count();

    last_time = now;

    _scheduler->update(delta_time);
    _event_queue->pop_all();
  }

  _logger->info("ended main loop");
}
  
void engine::terminate() {
  _logger->info("terminating modules...");

  // [NOTE] KAJ 2021-10-12 18:43: Maybe terminate modules in reverse order
  for (auto& [id, module] : _modules) {
    module->terminate();
  }

  _logger->info("finished terminating modules");
}

} // namespace sbx
