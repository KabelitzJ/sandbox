#ifndef LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_
#define LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_

#include <vector>
#include <utility>
#include <algorithm>
#include <ranges>

#include <libsbx/math/uuid.hpp>

namespace sbx::scenes {

class relationship {

public:

  relationship(math::uuid parent)
  : _parent{parent} {}

  relationship(const relationship& other) = default;

  auto parent() const noexcept -> math::uuid {
    return _parent;
  }

  auto set_parent(math::uuid parent) noexcept -> void {
    _parent = parent;
  }

  auto children() const noexcept -> const std::vector<math::uuid>& {
    return _children;
  }

  auto add_child(math::uuid child) noexcept -> void {
    _children.push_back(child);
  }

  auto remove_child(math::uuid child) noexcept -> void {
    std::ranges::remove(_children, child);
  }

private:

  math::uuid _parent;
  std::vector<math::uuid> _children;

}; // class relationship

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_
