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
  using component_type = Component;

  component_container() = default;
  ~component_container() = default;

  template<typename... Args>
  component_type& emplace_at(entity entity, Args&&... args);

private:
  sparse_set<entity, component_type> _components;

};

template<typename Component>
template<typename... Args>
inline auto component_container<Component>::emplace_at(entity entity, Args&&... args) -> component_type& {
  return _components.emplace_at(entity, std::forward<Args>(args)...);
}

} // namespace sbx

#endif // SBX_ECS_COMPONENT_CONTAINER_HPP_
