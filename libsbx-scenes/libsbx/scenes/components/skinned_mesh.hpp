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

  struct material {
    std::float_t metallic;
    std::float_t roughness;
    std::float_t flexibility;
    std::float_t anchor_height;
  }; // struct material

  struct submesh {
    std::uint32_t index{0};
    math::color tint{math::color::white()};
    skinned_mesh::material material{0.0f, 1.0f, 0.0f, 0.0f};
    graphics::image2d_handle albedo_texture{};
    graphics::image2d_handle normal_texture{};
    graphics::image2d_handle metallic_texture{};
    graphics::image2d_handle roughness_texture{};
  }; // struct submesh

  skinned_mesh(math::uuid mesh_id, math::uuid animation_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _animation_id{animation_id},
    _submeshes{submeshes} { }

  skinned_mesh(const math::uuid& mesh_id, math::uuid animation_id, std::uint32_t index = 0, const math::color& tint = math::color::white(), const skinned_mesh::material& material = skinned_mesh::material{}, const graphics::image2d_handle& albedo_texture = {})
  : _mesh_id{mesh_id},
    _animation_id{animation_id} {
    _submeshes.push_back(submesh{index, tint, material, albedo_texture});
  }

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
