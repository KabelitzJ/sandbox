#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <vector>
#include <cinttypes>

#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp> 

#include <libsbx/graphics/resource_storage.hpp>

#include <libsbx/graphics/images/image2d.hpp>

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
    std::uint32_t index{0};
    math::color tint{math::color::white()};
    static_mesh::material material{0.0f, 1.0f, 0.0f, 0.0f};
    graphics::image2d_handle albedo_texture{};
    graphics::image2d_handle normal_texture{};
    graphics::image2d_handle metallic_texture{};
    graphics::image2d_handle roughness_texture{};
  }; // struct submesh

  static_mesh(math::uuid mesh_id, const std::vector<submesh>& submeshes)
  : _mesh_id{mesh_id},
    _submeshes{submeshes} { }

  static_mesh(const math::uuid& mesh_id, std::uint32_t index = 0, const math::color& tint = math::color::white(), const static_mesh::material& material = static_mesh::material{}, const graphics::image2d_handle& albedo_texture = {}, const graphics::image2d_handle& normal_texture = {})
  : _mesh_id{mesh_id} {
    _submeshes.push_back(submesh{index, tint, material, albedo_texture, normal_texture});
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

  auto submesh_at(const std::uint32_t index) -> submesh& {
    utility::assert_that(index < _submeshes.size(), "Submesh index out of bounds");
    return _submeshes[index];
  }

private:

  math::uuid _mesh_id;
  std::vector<submesh> _submeshes;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
