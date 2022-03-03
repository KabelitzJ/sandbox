#include "registry.hpp"

namespace sbx {

registry::registry(registry&& other) noexcept
: _entities{std::move(other._entities)},
  _free_list{std::move(other._free_list)} { }

registry& registry::operator=(registry&& other) noexcept {
  _entities = std::move(other._entities);
  _free_list = std::move(other._free_list);

  return *this;
}

entity registry::create_entity() {
  if (!_free_list.empty()) {
    const auto index = _free_list.front();
    _free_list.pop();

    return _entities[index];
  }

  _entities.push_back({static_cast<entity::id_type>(_entities.size()), entity::version_type{0}});
  return _entities.back();
}

void registry::destroy_entity(const entity& entity) {
  const auto index = static_cast<size_type>(entity._id());
  _entities[index]._increment_version();
  _free_list.push(index);
}

bool registry::is_valid_entity(const entity& entity) const noexcept {
  const auto index = static_cast<size_type>(entity._id());
  return index < _entities.size() && _entities[index] == entity;
}

} // namespace sbx
