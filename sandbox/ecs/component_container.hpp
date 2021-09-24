#ifndef SBX_ECS_COMPONENT_CONTAINER_HPP_
#define SBX_ECS_COMPONENT_CONTAINER_HPP_

#include "entity.hpp"
#include "sparse_set.hpp"

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

  template<typename... Args>
  void assign(const entity entity, Args&&... args);

  void remove(const entity entity);

private:
  sparse_set<entity, Component> _components;

};


template<typename Component>
template<typename... Args>
void component_container<Component>::assign(const entity entity, Args&&... args) {
  _components.assign(entity, std::forward<Args>(args)...);
}

template<typename Component>
void component_container<Component>::remove(const entity entity) {
  _components.remove(entity);
}

} // namespace sbx

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
