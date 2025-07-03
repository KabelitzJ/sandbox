#include <libsbx/mesh/mesh_source.hpp>

#include <fstream>
#include <bit>

#include <fmt/format.h>

#include <libbase64.h>

#include <nlohmann/json.hpp>

#include <libsbx/io/read_file.hpp>

#include <libsbx/math/vector2.hpp>  
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/quaternion.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

namespace sbx::mesh {

mesh_source::mesh_source(const std::filesystem::path& path) {
  auto file = std::ifstream{path};

  auto json = nlohmann::json::parse(file);

  const auto& nodes = json["nodes"];
  const auto& meshes = json["meshes"];
  const auto& accessors = json["accessors"];
  const auto& buffer_views = json["bufferViews"];
  const auto& buffers = json["buffers"];
  
  for (const auto& node : nodes) {
    if (!node.contains("mesh")) {
      continue;
    }

    const auto mesh_index = node["mesh"].get<std::size_t>();

    const auto& mesh = meshes[mesh_index];

    auto mesh_name = path.stem().string();

    if (mesh.contains("name")) {
      mesh_name = mesh["name"].get<std::string>();
    }

    auto submesh = graphics::submesh{};

    // [NOTE] KAJ 2023-11-22 : This is a offset into the vertex buffer. We dont want to use this.
    submesh.vertex_offset = 0u;

    const auto& primitives = mesh["primitives"];
    
    for (const auto& primitive : primitives) {
      submesh.index_offset = static_cast<std::uint32_t>(_indices.size());

      const auto& attributes = primitive["attributes"];

      for (const auto& [name, index] : attributes.items()) {
        
      }
    }
  }
}

auto mesh_source::_load_attribute(const attribute_type type) -> void {

}

} // namespace sbx::mesh
