#ifndef LIBSBX_ANIMATIONS_MESH_HPP_
#define LIBSBX_ANIMATIONS_MESH_HPP_

#include <filesystem>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>

#include <libsbx/io/loader_factory.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/animations/vertex3d.hpp>
#include <libsbx/animations/skeleton.hpp>

namespace sbx::animations {

class mesh : public graphics::mesh<vertex3d> {

  using base = graphics::mesh<vertex3d>;

public:

  using mesh_data = graphics::mesh<vertex3d>::mesh_data;

  using base::mesh;

  mesh(const std::filesystem::path& path);

  ~mesh() override;

  auto skeleton() const -> const animations::skeleton& {
    return _skeleton;
  }

private:

  static auto _load(const std::filesystem::path& path) -> mesh_data;

  animations::skeleton _skeleton;

}; // class mesh

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_MESH_HPP_
