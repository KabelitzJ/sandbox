#ifndef LIBSBX_SCENES_COMPONENTS_MESH_COMPONENTS_HPP_
#define LIBSBX_SCENES_COMPONENTS_MESH_COMPONENTS_HPP_

#include <libsbx/models/mesh.hpp>

namespace sbx::scenes {

struct mesh_component {
  models::mesh::handle_type handle{};
}; // class mesh_component

} // namespace sbx::scenes

#endif // LIBSBX_SCENES_COMPONENTS_MESH_COMPONENTS_HPP_
