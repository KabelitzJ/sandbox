#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <vector>

#include <types/primitives.hpp>

#include "entity.hpp"

namespace sbx {

template<typename Entity, typename Component>
class sparse_set {

public:
  using component_type = Component;
  using entity_type = Entity;

  template<typename... Args>
  void assign(const entity_type entity, Args&&... args);

  void remove(const entity_type entity);

  bool contains(const entity_type entity) const noexcept;

private:
  std::vector<size_type> _entity_indices;
  std::vector<entity_type> _entity_list;
  std::vector<component_type> _component_list;

}; // class sparse_set


template<typename Entity, typename Component>
template<typename... Args>
void sparse_set<Entity, Component>::assign(const entity_type entity, Args&&... args) {
  const auto next = _entity_list.size();
  // [NOTE] KAJ 2021-09-24 09:29: Find out if to_integral is the right function here
  const auto position = entity_traits<entity_type>::to_integral(entity);

  if (position >= _entity_indices.size()) {
    _entity_indices.resize(position - 1, entity_traits<entity_type>::to_integral(null_entity));
  }

  _entity_indices.at(position) = next;
  _entity_list.push_back(entity);
  _component_list.emplace_back(Component{std::forward<Args>(args)...});
}

template<typename Entity, typename Component>
void sparse_set<Entity, Component>::remove(const entity_type entity) {
  const auto position = entity_traits<entity_type>::to_integral(entity);

  if (!contains(entity)) {
    return;
  }

  const auto index = _entity_indices.at(position);

  // [NOTE] KAJ 2021-09-24 11:34: Maybe intorduce a tombstone class to identify those whole
  _entity_indices.at(position) = entity_traits<entity_type>::to_integral(null_entity);

  std::swap(_entity_list.at(index), _entity_list.back());
  _entity_list.pop_back();

  std::swap(_component_list.at(index), _component_list.back());
  _component_list.pop_back();
}

template<typename Entity, typename Component>
bool sparse_set<Entity, Component>::contains(const entity_type entity) const noexcept {
  const auto position = entity_traits<Entity>::to_integral(entity);

  return position < _entity_indices.size() && _entity_indices.at(position) != entity_traits<entity_type>::to_integral(null_entity);
}

} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
