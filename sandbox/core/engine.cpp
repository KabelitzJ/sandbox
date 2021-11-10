#include "engine.hpp"

#include <chrono>

#include <types/primitives.hpp>

#include <utils/reverse_adaptor.hpp>

#include "logger.hpp"
#include "events.hpp"
#include "user.hpp"

namespace sbx {

engine::engine()
: _scene{std::make_unique<scene>()},
  _scheduler{std::make_unique<scheduler>()},
  _event_queue{std::make_unique<event_queue>()},
  _resource_cache{std::make_unique<resource_cache>()},
  _modules{} { }

engine::~engine() {
  
}

void engine::initialize() {
  logger::_initialize();

  scene_user::_scene = _scene.get();
  scheduler_user::_scheduler = _scheduler.get();
  event_queue_user::_event_queue = _event_queue.get();
  resource_cache_user::_resource_cache = _resource_cache.get();

  logger::info("Initializing modules...");

  for (auto& module : _modules) {
    module->initialize();
  }

  logger::info("Finished initializing modules");
}

void engine::start() {
  logger::info("Started main loop");

  _event_queue->add_listener<application_shutdown_event>([this](const auto& event){
    logger::info("Application shutdown (origin: {})", event.origin);
    _scheduler->terminate();
  });

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

  logger::info("Ended main loop");
}
  
void engine::terminate() {
  logger::info("Terminating modules...");

  for (auto& module : _modules) {
    module->terminate();
  }

  logger::info("Finished terminating modules");
}

} // namespace sbx
