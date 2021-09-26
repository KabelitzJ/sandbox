#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <array>
#include <limits>
#include <bitset>
#include <memory>

#include "entity.hpp"
#include "type_index.hpp"

namespace sbx {

class registry {

public:
  using id_type = std::size_t;

  registry();
  ~registry();

  [[nodiscard]] entity create_entity();
  
  void destoy_entity(entity entity);

private:

}; // class registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
