#include "entity_set.hpp"

namespace sbx {

bool entity_set::contains(const entity& entity) const noexcept {
  if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
    const auto index = entry->second;
    return index < _dense.size() && _dense[index] == entity;
  }

  return false;
}

void entity_set::remove(const entity& entity) {
  if (!contains(entity)) {
    return;
  }

  _swap_and_pop(entity);
}

void entity_set::_swap_and_pop(const entity& entity) {
  const auto index = _sparse.at(entity);

  _sparse.at(_dense.back()) = index;
  const auto& old_entity = std::exchange(_dense.at(index), _dense.back());

  _dense.pop_back();
  _sparse.erase(old_entity);
}

void entity_set::_emplace(const entity& entity) {
  const auto index = _dense.size();

  _sparse.emplace(std::make_pair(entity, index));
  _dense.push_back(entity);
}

entity_set::size_type entity_set::_index(const entity& entity) const {
  if (const auto entry = _sparse.find(entity); entry != _sparse.cend()) {
    return entry->second;
  }

  throw std::out_of_range{"Set does not contain entity"};
}

} // namespace sbx
