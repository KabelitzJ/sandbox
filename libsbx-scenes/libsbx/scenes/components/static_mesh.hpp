#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <vector>
#include <cinttypes>

#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp> 


namespace sbx::scenes {

class static_mesh final {

public:

  struct material {
    std::float_t metallic;
    std::float_t roughness;
    std::float_t flexibility;
    std::float_t anchor_height;
  }; // struct material

  struct submesh {
    std::uint32_t index;
    math::color tint{math::color::white};
    static_mesh::material material{0.0f, 1.0f, 0.0f, 0.0f};
    std::optional<math::uuid> albedo_texture{std::nullopt};
    std::optional<math::uuid> normal_texture{std::nullopt};
    std::optional<math::uuid> metallic_texture{std::nullopt};
    std::optional<math::uuid> roughness_texture{std::nullopt};
  }; // struct submesh

  static_mesh(math::uuid mesh_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _submeshes{submeshes} { }

  static_mesh(const math::uuid& mesh_id, const math::color& tint = math::color::white, const static_mesh::material& material = static_mesh::material{}, const std::optional<math::uuid>& albedo_texture = std::nullopt)
  : _mesh_id{mesh_id} {
    _submeshes.push_back(submesh{0, tint, material, albedo_texture, std::nullopt, std::nullopt, std::nullopt});
  }

  auto mesh_id() const noexcept -> math::uuid {
    return _mesh_id;
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

  auto submeshes() noexcept -> std::vector<submesh>& {
    return _submeshes;
  }

private:

  math::uuid _mesh_id;
  std::vector<submesh> _submeshes;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
