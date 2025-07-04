#ifndef LIBSBX_MESH_MESH_SOURCE_HPP_
#define LIBSBX_MESH_MESH_SOURCE_HPP_

#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <span>

#include <nlohmann/json.hpp>

#include <libsbx/utility/hashed_string.hpp>
#include <libsbx/utility/enum.hpp>

#include <libsbx/graphics/pipeline/mesh.hpp>

namespace sbx::mesh {

class mesh_source {

public:

  enum class attribute_type : std::uint8_t {
    position,
    normal,
    tangent,
    texcoord_0,
    color_0,
    joints_0,
    weights_0,
  }; // struct attribute_type

  enum class component_type : std::uint16_t {
    signed_byte,
    unsigned_byte,
    signed_short,
    unsigned_short,
    unsigned_int,
    floating_point
  }; // struct component_type

  enum class data_type : std::uint8_t {
    scalar,
    vec2,
    vec3,
    vec4,
    mat2,
    mat3,
    mat4
  }; // struct type

  struct attribute {
    // attribute_type attribute_type;
    mesh_source::component_type component_type;
    mesh_source::data_type data_type;
    std::vector<std::uint8_t> buffer;
  }; // struct attribute

  mesh_source(const std::filesystem::path& path);

  auto find_attribute(const attribute_type type) const -> const attribute& {
    if (const auto entry = _attributes.find(type); entry != _attributes.cend()) {
      return entry->second;
    }

    throw std::runtime_error{fmt::format("Mesh does not have attribute '{}", utility::to_underlying(type))};
  }

  auto submeshes() const -> const std::vector<graphics::submesh>& {
    return _submeshes;
  }

  auto indices() -> const std::vector<std::uint32_t>& {
    return _indices;
  }

private:

  std::unordered_map<attribute_type, attribute>  _attributes;
  std::vector<std::uint32_t> _indices;
  std::vector<graphics::submesh> _submeshes;

}; // class mesh_source

} // namespace sbx::mesh

#endif // LIBSBX_MESH_MESH_SOURCE_HPP_
