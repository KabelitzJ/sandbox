#include "registry.hpp"

#include <utility>

namespace sbx {

registry::registry(registry&& other) noexcept
: _entities{std::move(other._entities)},
  _free_entities{std::move(other._free_entities)} { }

registry& registry::operator=(registry&& other) noexcept {
  _entities = std::move(other._entities);
  _free_entities = std::move(other._free_entities);

  return *this;
}

entity registry::create_entity() {
  if (!_free_entities.empty()) {
    const auto index = _free_entities.front();
    _free_entities.pop();

    return _entities[index];
  }

  const auto id = static_cast<entity::id_type>(_entities.size());
  _entities.push_back(entity{id, entity::version_type{0}});

  return _entities.back();
}

void registry::destroy_entity(const entity& entity) {
  const auto index = static_cast<size_type>(entity._id());
  _entities[index]._increment_version();
  _free_entities.push(index);
}

bool registry::is_valid_entity(const entity& entity) const noexcept {
  const auto index = static_cast<size_type>(entity._id());
  return index < _entities.size() && _entities[index] == entity;
}

} // namespace sbx
