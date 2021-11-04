#include "render_system.hpp"

#include <glad/glad.h>

#include <core/events.hpp>

namespace sbx {

render_system::render_system(event_queue* event_queue)
: _event_queue{event_queue} { }

void render_system::initialize() {

}

void render_system::update([[maybe_unused]] time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);
}

void render_system::terminate() {
  
}

} // namespace sbx
