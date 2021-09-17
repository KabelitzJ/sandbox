#include "module.hpp"

namespace sbx {

module::module()
: _registry(nullptr),
  _systems() {

}

void module::_initialize() {
  for (auto& system : _systems) {
    system->initialize();
  }
}

} // namespace sbx
