#include "module.hpp"

namespace sbx {

module::module() {

}

void module::_initialize() {
  for (auto& system : _systems) {
    system->initialize();
  }
}

} // namespace sbx
