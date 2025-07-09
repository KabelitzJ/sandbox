#ifndef LIBSBX_ANIMATION_MESH_HPP_
#define LIBSBX_ANIMATION_MESH_HPP_

#include <filesystem>

#include <libsbx/utility/hash.hpp>

#include <libsbx/math/volume.hpp>
#include <libsbx/math/sphere.hpp>

#include <libsbx/io/loader_factory.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/animation/vertex3d.hpp>
#include <libsbx/animation/skeleton.hpp>

namespace sbx::animation {

class mesh : public graphics::mesh<vertex3d> {

  using base = graphics::mesh<vertex3d>;

public:

  using mesh_data = graphics::mesh<vertex3d>::mesh_data;

  using base::mesh;

  mesh(const std::filesystem::path& path);

  ~mesh() override;

  auto skeleton() const -> const animation::skeleton& {
    return _skeleton;
  }

private:

  static auto _load(const std::filesystem::path& path) -> mesh_data;

  animation::skeleton _skeleton;

}; // class mesh

} // namespace sbx::animation

#endif // LIBSBX_ANIMATION_MESH_HPP_
