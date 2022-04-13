#ifndef SBX_CORE_SCENE_GRAPH_HPP_
#define SBX_CORE_SCENE_GRAPH_HPP_

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include <ecs/registry.hpp>

namespace sbx {

class scene_graph {

public:

private:

  struct node {
    node* parent{nullptr};
    std::vector<std::unique_ptr<node>> children{};
    std::string name{};
  }; // struct node

  std::unordered_map<std::string, std::unique_ptr<node>> _nodes{};
  registry _registry{};

}; // class scene_graph

} // namespace sbx

#endif // SBX_CORE_SCENE_GRAPH_HPP_
