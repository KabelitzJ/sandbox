#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <libsbx/assets/asset.hpp>

#include <libsbx/models/mesh.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::scenes {

class static_mesh final {

public:

  static_mesh(assets::asset_id mesh_id, assets::asset_id texture_id)
  : _mesh_id{mesh_id}, 
    _texture_id{texture_id} { }

  auto mesh_id() const noexcept -> assets::asset_id {
    return _mesh_id;
  }

  auto texture_id() const noexcept -> assets::asset_id {
    return _texture_id;
  }

private:

  assets::asset_id _mesh_id;
  assets::asset_id _texture_id;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
