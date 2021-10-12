#include "engine.hpp"

#include <chrono>

#include <types/primitives.hpp>

namespace sbx {

engine::engine()
: _scheduler{std::make_unique<scheduler>()},
  _modules{} { }

engine::~engine() {

}

void engine::initialize() {
  module::_scheduler = _scheduler.get();

  for (auto& module : _modules) {
    module->initialize();
  }
}

void engine::start() {
  using clock = std::chrono::steady_clock;
  using duration = std::chrono::duration<fast_time>;

  auto last_time = clock::now();

  while (!_scheduler->is_empty()) {
    const auto now = clock::now();

    const auto delta_time = std::chrono::duration_cast<duration>(now - last_time).count();

    last_time = now;

    _scheduler->update(delta_time);
  }
}

} // namespace sbx
