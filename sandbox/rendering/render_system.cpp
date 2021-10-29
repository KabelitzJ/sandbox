#include "render_system.hpp"

#include <core/events.hpp>

namespace sbx {

render_system::render_system(event_queue* event_queue)
: _event_queue{event_queue} { }

void render_system::initialize() {
  _event_queue->add_listener<window_closed_event>([this](const auto&){
    finish();
  });
}

void render_system::update([[maybe_unused]] time delta_time) {

}

void render_system::finished() {

}

void render_system::aborted() {

}

} // namespace sbx
