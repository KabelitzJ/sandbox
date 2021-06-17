#include "scene.hpp"

namespace sbx {

struct relationship {
  entt::entity parent{entt::null};
  std::vector<entt::entity> children{};
};

scene::scene() : _registry(), _relations() {
  
}

scene::~scene() {
  for (const auto& [entity, ancestors] : _relations) {
    _registry.destroy(entity);
  }
}

entt::entity scene::add_node() {
  const auto entity = _registry.create();
  
  _relations.emplace(entity, std::vector<entt::entity>{});

  return entity;
}

entt::entity scene::add_node(entt::entity parent) {
  const auto entity = _registry.create();
  
  const auto entry = _relations.find(parent);

  if (entry == _relations.end()) {
    // maybe do something smarter here?
    throw;
  }

  auto ancestors = entry->second;
  ancestors.push_back(parent);

  _relations.emplace(entity, ancestors);

  return entity;
}

} // namespace sbx
