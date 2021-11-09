#include "scene.hpp"

#include <cassert>

#include "transform.hpp"
#include "uuid.hpp"

namespace sbx {

struct relationship {
  entity parent{null_entity};
  // [TODO] KAJ 2021-11-03 12:16 - Figure out if std::unordered_set gives a perfromance hit here
  std::unordered_set<entity> children{};
}; // struct relationship

scene::scene() 
: _registry{},
  _root{_registry.create_entity()} {
  _registry.emplace_component<relationship>(_root);
}

scene::~scene() {
  // [TODO] KAJ 2021-11-03 09:44 - Save the registry on disk before destructing it
  _registry.clear();
}

entity scene::create_entity(const entity parent) {
  auto entity = _registry.create_entity();

  auto& parent_relationship = parent != null_entity ? _registry.get_components<relationship>(parent) : _registry.get_components<relationship>(_root);

  // [NOTE] KAJ 2021-11-03 13:26 - Should never happen... But better be save
  assert(parent_relationship.children.find(entity) == parent_relationship.children.cend());

  auto& current_relationship = _registry.emplace_component<relationship>(entity, parent);

  current_relationship.parent = parent;

  parent_relationship.children.insert(entity);

  _registry.emplace_component<transform>(entity);

  _registry.emplace_component<uuid>(entity);

  return entity;
}

void scene::destroy_entity(const entity entity) {
  auto& current_relationship = _registry.get_components<relationship>(entity);

  auto& parent_relationship = _registry.get_components<relationship>(current_relationship.parent);

  // [NOTE] KAJ 2021-11-03 13:32 - Something went wrong when creating relationships
  assert(parent_relationship.children.find(entity) == parent_relationship.children.cend());

  parent_relationship.children.erase(entity);

  _registry.destroy_entity(entity);
}

} // namespace sbx
