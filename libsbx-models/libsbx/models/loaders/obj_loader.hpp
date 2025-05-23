#ifndef LIBSBX_MODLES_LOADERS_OBJ_LOADER_HPP_
#define LIBSBX_MODLES_LOADERS_OBJ_LOADER_HPP_

#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class obj_loader : public mesh::loader<obj_loader> {

  inline static const auto is_registered = register_extensions(".obj");

public:

  static auto load(const std::filesystem::path& path) -> mesh::mesh_data;

}; // class obj_loader

} // namespace sbx::models

#endif // LIBSBX_MODLES_LOADERS_OBJ_LOADER_HPP_
