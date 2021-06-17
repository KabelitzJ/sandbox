#ifndef SBX_CORE_SCENE_HPP_
#define SBX_CORE_SCENE_HPP_

#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

namespace sbx {

class scene {

public:
  scene();
  ~scene();

  entt::entity add_node();
  entt::entity add_node(entt::entity parent);

private:
  entt::registry _registry{};
  std::unordered_map<entt::entity, std::vector<entt::entity>> _relations{};

}; // class scene

} // namespace sbx

#endif // SBX_CORE_SCENE_HPP_
