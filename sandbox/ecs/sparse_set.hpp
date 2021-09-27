#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <vector>

#include <types/primitives.hpp>

#include "entity.hpp"

namespace sbx {

template<typename Entity, typename Allocator = std::allocator<Entity>>
class sparse_set {

public:
  using entity_traits = entity_traits<Entity>;
  using entity_type = typename entity_traits::value_type;


private:

}; // class sparse_set

} // namespace sbx

#endif // SBX_ECS_SPARSE_SET_HPP_
