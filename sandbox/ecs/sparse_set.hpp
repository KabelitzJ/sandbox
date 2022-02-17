#ifndef SBX_ECS_SPARSE_SET_HPP_
#define SBX_ECS_SPARSE_SET_HPP_

#include <memory>
#include <vector>

#include <meta/concepts.hpp>

#include "entity_traits.hpp"
#include "entity.hpp"

namespace sbx {

template<entity Entity, allocator<Entity> Allocator>
class basic_sparse_set {

  using allocator_traits = std::allocator_traits<Allocator>;
  using entity_traits = entity_traits<Entity>;
  using sparse_container_type = std::vector<typename allocator_traits::pointer, typename allocator_traits::template rebind_alloc<typename allocator_traits::pointer>>;
  using packed_container_type = std::vector<typename allocator_traits::value_type, typename allocator_traits::allocator_type>;

  inline constexpr auto page_size = std::size_t{4096};

public:

  using allocator_type = typename allocator_traits::allocator_type;
  using entity_type = typename entity_traits::entity_type;
  using version_type = typename entity_traits::version_type;
  using size_type = typename packed_container_type::size_type;
  using pointer = typename packed_container_type::const_pointer;

protected:

private:

  sparse_container_type _sparse{};
  packed_container_type _packed{};
  entity_type _free_list{};

}; // class basic_sparse_set

using sparse_set = basic_sparse_set<default_entity, std::allocator<default_entity>>;

} // namespace sbx

#include "sparse_set.inl"

#endif // SBX_ECS_SPARSE_SET_HPP_
