#ifndef DEMO_ENTITY_SET_HPP_
#define DEMO_ENTITY_SET_HPP_

#include <unordered_map>
#include <vector>

#include <utils/noncopyable.hpp>

#include "entity.hpp"

namespace sbx {

class entity_set : public noncopyable {

  using sparse_container_type = std::unordered_map<entity, std::size_t>;
  using dense_container_type = std::vector<entity>;

public:

  using size_type = std::size_t;

  entity_set() noexcept = default;

  virtual ~entity_set() = default;

  [[nodiscard]] bool contains(const entity& entity) const noexcept;

  void remove(const entity& entity);

protected:

  virtual void _swap_and_pop(const entity& entity);

  void _emplace(const entity& entity);

  size_type _index(const entity& entity) const;

private:

  sparse_container_type _sparse{};
  dense_container_type _dense{};

}; // class entity_set

} // namespace sbx

#endif // DEMO_ENTITY_SET_HPP_
