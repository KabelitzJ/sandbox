#include "engine.hpp"

#include <chrono>

#include <types/primitives.hpp>

namespace sbx {

engine::engine()
: _scheduler{std::make_unique<scheduler>()},
  _event_queue{std::make_unique<event_queue>()},
  _modules{} { }

engine::~engine() {

}

void engine::initialize() {
  module::_scheduler = _scheduler.get();
  module::_event_queue = _event_queue.get();

  for (auto& module : _modules) {
    module->initialize();
  }
}

void engine::start() {
  using clock = std::chrono::steady_clock;
  using duration = typename scheduler::duration_type;

  auto last_time = clock::now();

  while (!_scheduler->is_empty()) {
    const auto now = clock::now();

    const auto delta_time = std::chrono::duration_cast<duration>(now - last_time).count();

    last_time = now;

    _scheduler->update(delta_time);
    _event_queue->pop_all();
  }
}
  
void engine::terminate() {
  // [NOTE] KAJ 2021-10-12 18:43: Maybe terminate modules in reverse order
  for (auto& module : _modules) {
    module->terminate();
  }
}

} // namespace sbx
