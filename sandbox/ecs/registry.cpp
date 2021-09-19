#include "registry.hpp"

namespace sbx {

registry::registry()
: _component_id_counter(0) {
    
}

registry::~registry() {
  
}

entity registry::create_entity() {
  return entity{0, 0};
}

void registry::destoy_entity(const entity& entity) {
  (void)entity;
}

} // namespace sbx
