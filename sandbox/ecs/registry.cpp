#include "registry.hpp"

namespace sbx {

registry::registry()
: _component_id_counter(0),
  _entities(),
  _component_pools(),
  _component_masks() {
    
}

registry::~registry() {
  
}

entity registry::create_entity() {
  const auto entity = _entities.emplace_back(_entities.size(), 0);
  _component_masks.emplace_back();
  
  return entity;
}

void registry::destoy_entity(const entity& entity) {
  (void)entity;
}

} // namespace sbx
