#include "engine.hpp"

namespace sbx {

engine::engine()
: _registry(std::make_unique<registry>()),
  _modules() {


}

engine::~engine() {

}

void engine::start() {

}

void engine::initialize() {
  for (auto& module : _modules) {
    module->initialize();
    module->_initialize();
  }
}

} // namespace sbx
