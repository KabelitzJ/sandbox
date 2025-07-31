#ifndef LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
#define LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_

#include <vector>
#include <cinttypes>

#include <libsbx/math/color.hpp>
#include <libsbx/math/uuid.hpp> 

#include <libsbx/graphics/resource_storage.hpp>

#include <libsbx/graphics/images/image2d.hpp>

namespace sbx::scenes {

enum class material_type : std::uint8_t {
  opaque,
  masked,
  transparent
}; // enum class material_type

struct material {
  material_type type;
  math::color base_color{math::color::white()};
  std::float_t metallic{0.0f};
  std::float_t roughness{0.5f};
  std::float_t amnient_occlusion{1.0f};
  graphics::image2d_handle albedo;
  graphics::image2d_handle normal;
  graphics::image2d_handle mrao; // metallic, roughness, amnient occlusion
}; // struct material

class static_mesh final {

public:

  struct submesh {
    std::uint32_t index;
    math::uuid material;
  }; // struct submesh

  static_mesh(const math::uuid& mesh_id, const math::uuid& material, const std::uint32_t lod = 0u)
  : _mesh_id{mesh_id},
    _lod{lod},
    _submeshes{{0, material}} { }

  static_mesh(const math::uuid& mesh_id, const std::vector<submesh>& submeshes, const std::uint32_t lod = 0u)
  : _mesh_id{mesh_id},
    _lod{lod},
    _submeshes{submeshes} { }

  static_mesh(const math::uuid& mesh_id, std::initializer_list<submesh> submeshes, const std::uint32_t lod = 0u)
  : _mesh_id{mesh_id},
  _lod{lod},
    _submeshes{submeshes} { }

  auto mesh_id() const noexcept -> const math::uuid& {
    return _mesh_id;
  }

  auto lod() const noexcept -> std::uint32_t {
    return _lod;
  }

  auto submeshes() const noexcept -> const std::vector<submesh>& {
    return _submeshes;
  }

private:

  math::uuid _mesh_id;
  std::uint32_t _lod;
  std::vector<submesh> _submeshes;

}; // class static_mesh

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_STATIC_MESH_HPP_
