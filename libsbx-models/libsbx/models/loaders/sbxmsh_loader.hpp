#ifndef LIBSBX_MODELS_LOADERS_SBXMSH_HPP_
#define LIBSBX_MODELS_LOADERS_SBXMSH_HPP_

#include <libsbx/models/mesh.hpp>

namespace sbx::models {

class sbxmsh_loader : public mesh::loader<sbxmsh_loader> {

  inline static const auto is_registered = register_extensions(".sbxmsh");

public:

  static auto load(const std::filesystem::path& path) -> mesh::mesh_data;

}; // class obj_loader

} // namespace sbx::models

#endif // LIBSBX_MODELS_LOADERS_SBXMSH_HPP_
