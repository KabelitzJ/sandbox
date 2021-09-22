#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include <util/sparse_set.hpp>

#include "entity.hpp"

namespace sbx {

class basic_component_container {

public:
  basic_component_container() = default;
  virtual ~basic_component_container() = default;

}; // class basic_component_container


template<typename Component>
class component_container : public basic_component_container {

public:
  component_container() = default;
  ~component_container() = default;

private:
  sparse_set<entity, Component> _components;

};

} // namespace sbx

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
