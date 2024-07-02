#ifndef LIBSBX_PHYSICS_OCTREE_HPP_
#define LIBSBX_PHYSICS_OCTREE_HPP_

#include <range/v3/all.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/uuid.hpp>

#include <libsbx/memory/static_vector.hpp>

namespace sbx::physics {


template<typename Type, std::size_t Threshold = 16u, std::size_t Depth = 8u>
class octree {

  inline static constexpr auto threshold = Threshold;
  inline static constexpr auto depth = Depth;

  struct node {
    
    using id = std::uint32_t;

    inline constexpr static auto null = static_cast<id>(-1);

    // memory::static_vector<std::pair<Type, box>, threshold> values;
    std::array<id, 8u> children;
  
  }; // struct node

public:

  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  octree(const box& bounds) noexcept {
    _root = 0u;
    _nodes.push_back(node{});
  }

private:

  node::id _root;
  std::vector<node> _nodes;

}; // class octree

} // namespace sbx::physics

#endif // LIBSBX_PHYSICS_OCTREE_HPP_
