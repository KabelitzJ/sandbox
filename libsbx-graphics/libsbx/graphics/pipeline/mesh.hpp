#ifndef LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include <libsbx/utility/hashed_string.hpp>
#include <libsbx/utility/exception.hpp>

#include <libsbx/math/volume.hpp>

#include <libsbx/graphics/buffers/buffer.hpp>
#include <libsbx/graphics/buffers/storage_buffer.hpp>

#include <libsbx/graphics/commands/command_buffer.hpp>

#include <libsbx/graphics/pipeline/vertex_input_description.hpp>

#include <libsbx/graphics/resource_storage.hpp>

namespace sbx::graphics {

// template<typename Type>
// using vertex_buffer = typed_buffer<Type, (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

// template<typename Type>
// using index_buffer = typed_buffer<Type, (VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT>;

struct submesh {
  std::uint32_t index_count;
  std::uint32_t index_offset;
  std::uint32_t vertex_offset;
  math::volume bounds;
  math::matrix4x4 transform;
  utility::hashed_string name;
}; // struct submesh

template<vertex Vertex>
class mesh {

  static_assert(std::is_copy_assignable_v<math::matrix4x4>, "The matrix4x4 type must be copy assignable for mesh operations.");
  static_assert(std::is_copy_constructible_v<math::matrix4x4>, "The matrix4x4 type must be copy constructible for mesh operations.");
  static_assert(std::is_move_assignable_v<math::matrix4x4>, "The matrix4x4 type must be move assignable for mesh operations.");
  static_assert(std::is_move_constructible_v<math::matrix4x4>, "The matrix4x4 type must be move constructible for mesh operations.");

public:

  using vertex_type = Vertex;
  // using vertex_buffer_type = vertex_buffer<Vertex>;

  using index_type = std::uint32_t;
  // using index_buffer_type = index_buffer<index_type>;

  struct mesh_data {
    std::vector<vertex_type> vertices;
    std::vector<index_type> indices;
    std::vector<graphics::submesh> submeshes;
    // std::vector<math::volume> submesh_bounds;
    // std::vector<math::matrix4x4> submesh_transforms;
    // std::unordered_map<utility::hashed_string, std::uint32_t> submesh_names;
    math::volume bounds;
  }; // struct mesh_data

  mesh(const std::vector<vertex_type>& vertices, const std::vector<index_type>& indices, const math::volume& bounds = math::volume{});

  mesh(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices, const math::volume& bounds = math::volume{});

  mesh(const mesh& other) noexcept = delete;

  virtual ~mesh();

  auto render(graphics::command_buffer& command_buffer, std::uint32_t instance_count = 1u) const -> void;

  auto render_submesh(graphics::command_buffer& command_buffer, std::uint32_t submesh_index, std::uint32_t instance_count = 1u) const -> void;

  auto address() const -> std::uint64_t;

  auto bind(graphics::command_buffer& command_buffer) const -> void;

  auto render_submesh_indirect(graphics::storage_buffer& buffer, std::uint32_t offset, std::uint32_t submesh_index, std::uint32_t instance_count = 1u) const -> void;

  auto submeshes() const noexcept -> const std::vector<graphics::submesh>&;

  auto submesh(std::uint32_t submesh_index) const -> const graphics::submesh& {
    return _submeshes.at(submesh_index);
  }

  auto submesh_index(const utility::hashed_string& name) const -> std::uint32_t {
    const auto entry = std::ranges::find(_submeshes, name, &graphics::submesh::name);

    if  (entry == _submeshes.end()) {
      throw utility::runtime_error{"Submesh '{}' not found", name.str()};
    }
  
    return std::distance(_submeshes.begin(), entry);
  }

  auto submesh_bounds(std::uint32_t submesh_index) const -> const math::volume& {
    return _submeshes.at(submesh_index).bounds;
  }

  auto submesh_transform(std::uint32_t submesh_index) const -> const math::matrix4x4& {
    return _submeshes.at(submesh_index).transform;
  }

  auto submesh_names() const -> std::unordered_map<utility::hashed_string, std::uint32_t> {
    auto names = std::unordered_map<utility::hashed_string, std::uint32_t>{};

    for (std::size_t i = 0; i < _submeshes.size(); ++i) {
      names.emplace(_submeshes[i].name, static_cast<std::uint32_t>(i));
    }

    return names;
  }

  auto bounds() const -> const math::volume& {
    return _bounds;
  }

protected:

  mesh(mesh_data&& mesh_data);

  auto _upload_vertices(const std::vector<vertex_type>& vertices, const std::vector<index_type>& indices) -> void;

  auto _upload_vertices(std::vector<vertex_type>&& vertices, std::vector<index_type>&& indices) -> void;

  auto _calculate_bounds_from_submeshes(math::volume&& bounds) const -> math::volume;

  // vertex_buffer_type _vertex_buffer;
  // index_buffer_type _index_buffer;
  buffer_handle _vertex_buffer;
  buffer_handle _index_buffer;

  std::vector<graphics::submesh> _submeshes;
  // std::vector<math::volume> _submesh_bounds;
  // std::vector<math::matrix4x4> _submesh_transforms;
  // std::unordered_map<utility::hashed_string, std::uint32_t> _submesh_names;

  math::volume _bounds;

}; // class mesh

} // namespace sbx::graphics

#include <libsbx/graphics/pipeline/mesh.ipp>

#endif // LIBSBX_GRAPHICS_PIPELINE_MESH_HPP_
