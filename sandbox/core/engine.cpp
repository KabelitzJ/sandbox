#include "engine.hpp"

#include <chrono>

namespace sbx {

engine::engine()
: _registry{std::make_unique<basic_registry<entity>>()},
  _scheduler{std::make_unique<scheduler<fast_time>>()},
  _modules{} {


}

engine::~engine() {

}

void engine::initialize() {
  for (auto& module : _modules) {
    module->initialize();
    module->_initialize();
  }
}

void engine::start() {
  using clock = std::chrono::steady_clock;
  using duration = std::chrono::duration<fast_time>;

  auto last_time = clock::now();

  while (true) {
    const auto now = clock::now();

    const auto delta = std::chrono::duration_cast<duration>(now - last_time).count();

    last_time = now;

    _scheduler->update(delta);
  }
}

} // namespace sbx
