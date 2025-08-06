#ifndef LIBSBX_SCENES_COMPONENTS_SKINNED_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_SKINNED_MESH_HPP_

#include <vector>
#include <cinttypes>

#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp> 

#include <libsbx/graphics/resource_storage.hpp>

#include <libsbx/graphics/images/image2d.hpp>

#include <libsbx/scenes/components/static_mesh.hpp>

namespace sbx::scenes {

struct animation_state {
  float current_time = 0.0f;
  float speed = 1.0f;
  bool looping = true;
}; // struct animation_state

class skinned_mesh final {

public:

  struct submesh {
    std::uint32_t index;
    math::uuid material;
  }; // struct submesh

  skinned_mesh(const math::uuid mesh_id, const math::uuid animation_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _animation_id{animation_id},
    _submeshes{submeshes} { }

  skinned_mesh(const math::uuid mesh_id, const math::uuid animation_id, const math::uuid material)
  : _mesh_id{mesh_id},
    _animation_id{animation_id},
    _submeshes{{0, material}} { }

  auto mesh_id() const noexcept -> math::uuid {
    return _mesh_id;
  }

  auto animation_id() const noexcept -> math::uuid {
    return _animation_id;
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

  auto submeshes() noexcept -> std::vector<submesh>& {
    return _submeshes;
  }

private:

  math::uuid _mesh_id;
  math::uuid _animation_id;
  std::vector<submesh> _submeshes;

}; // class skinned_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_SKINNED_MESH_HPP_
