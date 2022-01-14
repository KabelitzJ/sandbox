#include "engine.hpp"

namespace sbx {

engine::engine()
: _window{} { }

engine::~engine() { }

void engine::start() {
  while (true) {
    _window.swap_buffers();
  }
}

} // namespace sbx
