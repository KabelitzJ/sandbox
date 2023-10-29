#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <libsbx/math/color.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::scenes {

class static_mesh final {

public:

  static_mesh(assets::asset_id mesh_id, assets::asset_id texture_id, const math::color& tint = math::color{1.0f, 1.0f, 1.0f, 1.0f})
  : _mesh_id{mesh_id}, 
    _texture_id{texture_id},
    _tint{tint} { }

  auto mesh_id() const noexcept -> assets::asset_id {
    return _mesh_id;
  }

  auto texture_id() const noexcept -> assets::asset_id {
    return _texture_id;
  }

  auto tint() const noexcept -> const math::color& {
    return _tint;
  }

private:

  assets::asset_id _mesh_id;
  assets::asset_id _texture_id;
  math::color _tint;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
