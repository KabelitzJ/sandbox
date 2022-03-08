#ifndef SBX_ECS_REGISTRY_HPP_
#define SBX_ECS_REGISTRY_HPP_

#include <vector>
#include <queue>

#include "entity.hpp"

namespace sbx {

class registry {

public:

  using size_type = std::size_t;

  registry() = default;

  registry(const registry&) = delete;

  registry(registry&&) noexcept;

  ~registry() = default;

  registry& operator=(const registry&) = delete;

  registry& operator=(registry&&) noexcept;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

private:

  std::vector<entity> _entities{};
  std::queue<size_type> _free_entities{};

}; // class registry

} // namespace sbx

#endif // SBX_ECS_REGISTRY_HPP_
