#include "engine.hpp"

namespace sbx {

engine::engine()
: _modules() {

}

engine::~engine() {

}

void engine::start() {

}

void engine::initialize() {
  for (auto& module : _modules) {
    module->_initialize();
    module->initialize();
  }
}

} // namespace sbx
