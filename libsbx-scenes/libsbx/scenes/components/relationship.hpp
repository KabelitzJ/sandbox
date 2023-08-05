#ifndef LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_
#define LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_

#include <libsbx/math/uuid.hpp>

#include <vector>
#include <ranges>

namespace sbx::scenes {

class relationship final {

public:

  relationship(math::uuid parent)
  : _parent{parent} { }

  auto parent() const noexcept -> math::uuid {
    return _parent;
  }

  auto set_parent(math::uuid parent) noexcept -> void {
    _parent = parent;
  }

  auto children() const noexcept -> const std::vector<math::uuid>& {
    return _children;
  }

  auto children() noexcept -> std::vector<math::uuid>& {
    return _children;
  }

  auto add_child(math::uuid child) -> void {
    _children.push_back(child);
  }

  auto remove_child(math::uuid child) -> void {
    std::ranges::remove(_children, child);
  }

private:

  math::uuid _parent;
  std::vector<math::uuid> _children;

}; // class relationship

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_RELATIONSHIP_HPP_
