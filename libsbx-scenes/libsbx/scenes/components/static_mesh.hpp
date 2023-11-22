#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <libsbx/math/color.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::scenes {

class static_mesh final {

public:

  static_mesh(assets::asset_id mesh_id, assets::asset_id texture_id, const std::vector<std::uint32_t>& submesh_indices = {0u})
  : _mesh_id{mesh_id},
    _submesh_indices{submesh_indices},
    _texture_id{texture_id} { }

  auto mesh_id() const noexcept -> assets::asset_id {
    return _mesh_id;
  }

  auto submesh_indices() const noexcept -> const std::vector<std::uint32_t>& {
    return _submesh_indices;
  }

  auto texture_id() const noexcept -> assets::asset_id {
    return _texture_id;
  }

private:

  assets::asset_id _mesh_id;
  std::vector<std::uint32_t> _submesh_indices;
  assets::asset_id _texture_id;


}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
