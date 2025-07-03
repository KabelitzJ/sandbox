#ifndef LIBSBX_MESH_MESH_SOURCE_HPP_
#define LIBSBX_MESH_MESH_SOURCE_HPP_

#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <span>

#include <libsbx/utility/hashed_string.hpp>
#include <libsbx/utility/enum.hpp>

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
    count
  }; // struct attribute_type

  enum class accessor_type : std::uint8_t {
    vec2,
    vec3,
    vec4
  }; // struct accessor_type

  enum class component_type : std::uint8_t {
    floating_point,
    unsigned_byte,
    unsigned_int
  }; // enum class component_type

  struct attribute {
    attribute_type type;
    std::uint32_t component_count;
    std::uint32_t component_size;
    std::vector<std::uint8_t> buffer;
  }; // struct attribute

  mesh_source(const std::filesystem::path& path);

  auto attribute_buffer(const attribute_type type) const -> const std::vector<std::uint8_t>& {
    return _attributes[utility::to_underlying(type)].buffer;
  }

  auto submeshes() const -> const std::vector<graphics::submesh>& {
    return _submeshes;
  }

  auto indices() -> const std::vector<std::uint32_t>& {
    return _indices;
  }

private:

  auto _load_attribute(const attribute_type type) -> void;

  std::array<attribute, utility::to_underlying(attribute_type::count)>  _attributes;
  std::vector<graphics::submesh> _submeshes;
  std::vector<std::uint32_t> _indices;

}; // class mesh_source

} // namespace sbx::mesh

#endif // LIBSBX_MESH_MESH_SOURCE_HPP_
