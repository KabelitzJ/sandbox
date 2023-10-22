#ifndef LIBSBX_MODELS_MESH_HPP_
#define LIBSBX_MODELS_MESH_HPP_

#include <filesystem>
#include <cmath>
#include <cstdint>
#include <string>
#include <functional>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <tiny_obj_loader.h>

#include <libsbx/utility/hash.hpp>

#include <libsbx/assets/asset.hpp>

#include <libsbx/math/vector2.hpp>
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/vector4.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/color.hpp>

#include <libsbx/graphics/pipeline/pipeline.hpp>
#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>
#include <libsbx/graphics/pipeline/mesh.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/descriptor/descriptor_handler.hpp>

#include <libsbx/graphics/buffer/uniform_handler.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

class mesh : public graphics::mesh<vertex3d>, public assets::asset<assets::asset_type::mesh> {

public:

  struct mesh_data {
    std::vector<vertex3d> vertices;
    std::vector<std::uint32_t> indices;
  }; // struct mesh_data

  template<typename Derived>
  class loader {

  protected:

    template<typename... Extensions>
    static auto register_extensions(Extensions&&... extensions) -> bool {
      ((mesh::_loaders()[extensions] = &Derived::load), ...);

      return true;
    }

  }; // struct loader

  mesh(const std::filesystem::path& path);

  ~mesh() override;
  
private:

  using loader_container_type = std::unordered_map<std::string, std::function<mesh_data(const std::filesystem::path&)>>;

  static auto _loaders() -> loader_container_type& {
    static auto loaders = loader_container_type{};
    return loaders;
  }

}; // class mesh

} // namespace sbx::models

#endif // LIBSBX_MODELS_MESH_HPP_
