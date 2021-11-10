#include "render_system.hpp"

#include <glad/glad.h>

#include <core/events.hpp>

namespace sbx {

void render_system::initialize() {

}

void render_system::update([[maybe_unused]] time delta_time) {
  glClear(GL_COLOR_BUFFER_BIT);
}

void render_system::terminate() {
  
}

} // namespace sbx
