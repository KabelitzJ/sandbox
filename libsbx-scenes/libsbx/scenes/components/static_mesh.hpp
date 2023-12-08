#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <vector>
#include <cinttypes>

#include <libsbx/math/color.hpp>

#include <libsbx/assets/asset.hpp>

namespace sbx::scenes {

class static_mesh final {

public:

  struct submesh {
    std::uint32_t index;
    assets::asset_id texture_id;
    math::color tint{1.0f, 1.0f, 1.0f, 1.0f};
    std::float_t flexibility{0.0f};
    std::float_t anchor_height{0.0f};
  }; // struct submesh

  static_mesh(assets::asset_id mesh_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _submeshes{submeshes} { }

  auto mesh_id() const noexcept -> assets::asset_id {
    return _mesh_id;
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

private:

  assets::asset_id _mesh_id;
  std::vector<submesh> _submeshes;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
