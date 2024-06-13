#ifndef LIBSBX_SCENES_COMPONENTS_GIZMO_HPP_
#define LIBSBX_SCENES_COMPONENTS_GIZMO_HPP_

#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp> 

namespace sbx::scenes {

class gizmo final {

public:

  gizmo(math::uuid mesh_id, std::uint32_t submesh_index, const math::color& tint)
  : _mesh_id{mesh_id},
    _submesh_index{submesh_index},
    _tint{tint} { }

  auto mesh_id() const noexcept -> math::uuid {
    return _mesh_id;
  }

  auto submesh_index() const noexcept -> std::uint32_t {
    return _submesh_index;
  }

  auto tint() const noexcept -> const math::color& {
    return _tint;
  }

  auto tint() noexcept -> math::color& {
    return _tint;
  }

private:

  math::uuid _mesh_id;
  std::uint32_t _submesh_index;
  math::color _tint;

}; // class gizmo

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_GIZMO_HPP_
