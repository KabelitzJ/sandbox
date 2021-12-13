#include "engine.hpp"

#include <chrono>

#include <types/primitives.hpp>

#include "logger.hpp"
#include "events.hpp"

namespace sbx {

engine::engine()
: _scene{std::make_unique<scene>()},
  _scheduler{std::make_unique<scheduler>()},
  _event_queue{std::make_unique<event_queue>()},
  _resource_cache{std::make_unique<resource_cache>()},
  _input{std::make_unique<input>(_event_queue.get())},
  _modules{},
  _has_focus{false} { }

engine::~engine() {
  
}

void engine::initialize() {
  logger::_initialize();

  module::_scheduler = _scheduler.get();
  module::_event_queue = _event_queue.get();
  
  system::_scene = _scene.get();
  system::_event_queue = _event_queue.get();
  system::_resource_cache = _resource_cache.get();
  system::_input = _input.get();

  _event_queue->add_listener<window_closed_event>([this](const auto& event){
    logger::info("Application shutdown (reason: {})", event.reason);
    _scheduler->terminate();
  });

  _event_queue->add_listener<window_focused_event>([this](const auto& event){
    _has_focus = event.has_focus;
  });

  logger::info("Initializing modules...");

  for (auto& module : _modules) {
    module->initialize();
  }

  logger::info("Finished initializing modules");
}

void engine::start() {
  logger::info("Started main loop");

  using clock = std::chrono::steady_clock;
  using duration = std::chrono::duration<time>;

  auto last_time = clock::now();

  while (!_scheduler->is_empty()) {
    const auto now = clock::now();
    const auto delta_time = std::chrono::duration_cast<duration>(now - last_time).count();
    last_time = now;

    // [TODO] KAJ 2021-11-11 21:39 - Figure out how to just update the update_system when window has lost focus
    //                               (needed because of glfwPollEvents)

    _event_queue->pop_all();
    _scheduler->update(delta_time);
  }

  logger::info("Ended main loop");
}
  
void engine::terminate() {
  logger::info("Terminating modules...");

  // [TODO] KAJ 2021-11-10 19:41 - Reversed range based for loop??

  for (auto it = _modules.rbegin(); it != _modules.rend(); ++it) {
    (*it)->terminate();
  }

  logger::info("Finished terminating modules");
}

} // namespace sbx
