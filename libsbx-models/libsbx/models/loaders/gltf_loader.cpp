#include <libsbx/models/loaders/gltf_loader.hpp>

#include <filesystem>
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

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

// [NOTE] KAJ 2024-06-19 : Taken from glTF-2.0 specifications (https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types)

struct component {
  std::size_t id;
  std::size_t size;
}; // struct component

struct component_type {
  inline static constexpr auto signed_byte = component{5120u, 8u};
  inline static constexpr auto unsigned_byte = component{5121u, 8u};
  inline static constexpr auto signed_short = component{5122u, 16u};
  inline static constexpr auto unsigned_short = component{5123u, 16u};
  inline static constexpr auto unsigned_int = component{5125u, 32u};
  inline static constexpr auto floating_point = component{5126u, 32u};
}; // struct component_type

struct components_count {
  inline static constexpr auto scalar = std::size_t{1u};
  inline static constexpr auto vec2 = std::size_t{2u};
  inline static constexpr auto vec3 = std::size_t{3u};
  inline static constexpr auto vec4 = std::size_t{4u};
  inline static constexpr auto mat2 = std::size_t{4u};
  inline static constexpr auto mat3 = std::size_t{9u};
  inline static constexpr auto mat4 = std::size_t{16u};
}; // struct type

static auto _decode_buffer(std::size_t index, const std::filesystem::path& path, std::unordered_map<std::size_t, std::vector<std::uint8_t>>& buffers, const nlohmann::json& json) -> const std::vector<std::uint8_t>& {
  if (const auto entry = buffers.find(index); entry != buffers.cend()) {
    return entry->second;
  }

  const auto byte_length = json["byteLength"].get<std::size_t>();
  const auto uri = json["uri"].get<std::string>();

  auto buffer = std::vector<std::uint8_t>{};
  buffer.resize(byte_length);

  if (uri.starts_with("data:")) {
    auto start = uri.find("data:application");

    if (start == std::string::npos) {
      throw std::runtime_error{fmt::format("Invalid buffer: {}", uri.substr(0, uri.find(',')))};
    }

    start = uri.find("base64,");

    if (start == std::string::npos) {
      throw std::runtime_error{fmt::format("Invalid buffer encoding: {}", uri.substr(0, uri.find(',')))};
    }

    const auto buffer_base64 = uri.substr(start + 7u);

    auto buffer_size_out = std::size_t{};

    if (!base64_decode(buffer_base64.data(), buffer_base64.size(), reinterpret_cast<char*>(buffer.data()), &buffer_size_out, 0)) {
      throw std::runtime_error{"Failed to decode base64 buffer"};
    }

    if (buffer_size_out != buffer.size()) {
      throw std::runtime_error{fmt::format("Decoded buffer size does not match expected size: {} != {}", buffer_size_out, buffer.size())};
    }
  } else {
    const auto bin_path = path / std::filesystem::path{uri};

    if (!std::filesystem::exists(bin_path)) {
      throw std::runtime_error{fmt::format("Binary file does not exist: {}", bin_path.string())};
    }

    buffer = io::read_file(bin_path);

    if (byte_length != buffer.size()) {
      throw std::runtime_error{fmt::format("Buffer size does not match expected size: {} != {}", byte_length, buffer.size())};
    }
  }

  return buffers.insert({index, std::move(buffer)}).first->second;
}

static auto _get_transform(const nlohmann::json& node) -> math::matrix4x4 {
  auto transform = math::matrix4x4::identity;

  if (node.contains("translation")) {
    const auto& translation = node["translation"];
    const auto x = translation[0].get<std::double_t>();
    const auto z = translation[1].get<std::double_t>();
    const auto y = translation[2].get<std::double_t>();

    transform = transform * math::matrix4x4::translated(math::matrix4x4::identity, math::vector3{x, y, z});
  }

  if (node.contains("rotation")) {
    const auto& rotation = node["rotation"];
    const auto x = rotation[0].get<std::double_t>();
    const auto z = rotation[1].get<std::double_t>();
    const auto y = rotation[2].get<std::double_t>();
    const auto w = rotation[3].get<std::double_t>();

    transform = transform * math::quaternion{x, y, z, w}.to_matrix();
  }

  if (node.contains("scale")) {
    const auto& scale = node["scale"];
    const auto x = scale[0].get<std::double_t>();
    const auto z = scale[1].get<std::double_t>();
    const auto y = scale[2].get<std::double_t>();

    transform = transform * math::matrix4x4::scaled(math::matrix4x4::identity, math::vector3{x, y, z});
  }

  if (node.contains("matrix")) {
    const auto& matrix = node["matrix"];
    const auto m00 = matrix[0].get<std::double_t>();
    const auto m01 = matrix[1].get<std::double_t>();
    const auto m02 = matrix[2].get<std::double_t>();
    const auto m03 = matrix[3].get<std::double_t>();
    const auto m10 = matrix[4].get<std::double_t>();
    const auto m11 = matrix[5].get<std::double_t>();
    const auto m12 = matrix[6].get<std::double_t>();
    const auto m13 = matrix[7].get<std::double_t>();
    const auto m20 = matrix[8].get<std::double_t>();
    const auto m21 = matrix[9].get<std::double_t>();
    const auto m22 = matrix[10].get<std::double_t>();
    const auto m23 = matrix[11].get<std::double_t>();
    const auto m30 = matrix[12].get<std::double_t>();
    const auto m31 = matrix[13].get<std::double_t>();
    const auto m32 = matrix[14].get<std::double_t>();
    const auto m33 = matrix[15].get<std::double_t>();

    transform = math::matrix4x4{
      m00, m10, m20, m30,
      m01, m11, m21, m31,
      m02, m12, m22, m32,
      m03, m13, m23, m33,
    };
  }

  return transform;
}

auto gltf_loader::load(const std::filesystem::path& path) -> mesh::mesh_data {
  auto data = mesh::mesh_data{};

  auto decoded_buffers = std::unordered_map<std::size_t, std::vector<std::uint8_t>>{};

  auto file = std::ifstream{path};

  auto json = nlohmann::json::parse(file);

  const auto& nodes = json["nodes"];
  const auto& meshes = json["meshes"];
  const auto& accessors = json["accessors"];
  const auto& buffer_views = json["bufferViews"];
  const auto& buffers = json["buffers"];

  auto unique_vertices = std::unordered_map<vertex3d, std::uint32_t>{};

  for (const auto& node : nodes) {
    if (!node.contains("mesh")) {
      continue;
    }

    const auto mesh_index = node["mesh"].get<std::size_t>();

    const auto transform = _get_transform(node);

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
      submesh.index_offset = static_cast<std::uint32_t>(data.indices.size());

      const auto& attributes = primitive["attributes"];

      // [NOTE] KAJ 2024-03-20 : We need to check if the mesh contains the required attributes.

      if (!attributes.contains("POSITION")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'POSITION'", mesh_name, path.string())};
      }

      if (!attributes.contains("NORMAL")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'NORMAL'", mesh_name, path.string())};
      }

      if (!attributes.contains("TEXCOORD_0")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'TEXCOORD_0'", mesh_name, path.string())};
      }

      if (!attributes.contains("TANGENT")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'TANGENT'", mesh_name, path.string())};
      }

      if (!primitive.contains("indices")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'indices'", mesh_name, path.string())};
      }

      const auto positions_index = attributes["POSITION"].get<std::size_t>();
      const auto normals_index = attributes["NORMAL"].get<std::size_t>();
      const auto uvs_index = attributes["TEXCOORD_0"].get<std::size_t>();
      const auto tangents_index = attributes["TANGENT"].get<std::size_t>();
      const auto indices_index = primitive["indices"].get<std::size_t>();

      // [NOTE] KAJ 2024-03-20 : Here we get the accessors for the positions, normals, uvs and indices.

      const auto& positions_accessor = accessors[positions_index];
      const auto& normals_accessor = accessors[normals_index];
      const auto& uvs_accessor = accessors[uvs_index];
      const auto& tangents_accessor = accessors[tangents_index];
      const auto& indices_accessor = accessors[indices_index];

      // [NOTE] KAJ 2024-03-20 : We need to check if the accessors have the correct component type and type

      if (positions_accessor["componentType"].get<std::size_t>() != component_type::floating_point.id || positions_accessor["type"].get<std::string>() != "VEC3") {
        throw std::runtime_error{"Invalid component type or type for positions accessor"};
      }

      const auto& positions_accessor_min = positions_accessor["min"];
      const auto& positions_accessor_max = positions_accessor["max"];

      const auto min = math::vector3{
        positions_accessor_min[0u].get<std::float_t>(), 
        positions_accessor_min[1u].get<std::float_t>(), 
        positions_accessor_min[2u].get<std::float_t>()
      };
      
      const auto max = math::vector3{
        positions_accessor_max[0u].get<std::float_t>(), 
        positions_accessor_max[1u].get<std::float_t>(), 
        positions_accessor_max[2u].get<std::float_t>()
      };

      submesh.bounds = math::volume{min, max};

      if (normals_accessor["componentType"].get<std::size_t>() != component_type::floating_point.id || normals_accessor["type"].get<std::string>() != "VEC3") {
        throw std::runtime_error{"Invalid component type or type for normals accessor"};
      }

      if (uvs_accessor["componentType"].get<std::size_t>() != component_type::floating_point.id || uvs_accessor["type"].get<std::string>() != "VEC2") {
        throw std::runtime_error{"Invalid component type or type for uvs accessor"};
      }

      if (tangents_accessor["componentType"].get<std::size_t>() != component_type::floating_point.id || tangents_accessor["type"].get<std::string>() != "VEC4") {
        throw std::runtime_error{"Invalid component type or type for tangents accessor"};
      }

      // [TODO] KAJ 2024-03-20 : We should enable different component types for the indices accessor. For now we only support std::uint16_t.

      if ((indices_accessor["componentType"].get<std::size_t>() != component_type::unsigned_short.id && indices_accessor["componentType"].get<std::size_t>() != component_type::unsigned_int.id) || indices_accessor["type"].get<std::string>() != "SCALAR") {
        throw std::runtime_error{"Invalid component type or type for indices accessor"};
      }

      const auto positions_count = positions_accessor["count"].get<std::size_t>();
      const auto normals_count = normals_accessor["count"].get<std::size_t>();
      const auto uvs_count = uvs_accessor["count"].get<std::size_t>();
      const auto tangents_count = tangents_accessor["count"].get<std::size_t>();
      const auto indices_count = indices_accessor["count"].get<std::size_t>();

      if (positions_count != normals_count || positions_count != uvs_count || positions_count != tangents_count) {
        throw std::runtime_error{"Mismatching counts for positions, normals and uvs accessors"};
      }

      const auto positions_buffer_view_index = positions_accessor["bufferView"].get<std::size_t>();
      const auto normals_buffer_view_index = normals_accessor["bufferView"].get<std::size_t>();
      const auto uvs_buffer_view_index = uvs_accessor["bufferView"].get<std::size_t>();
      const auto tangents_buffer_view_index = tangents_accessor["bufferView"].get<std::size_t>();
      const auto indices_buffer_view_index = indices_accessor["bufferView"].get<std::size_t>();

      // [NOTE] KAJ 2024-03-20 : Here we get the buffer views for the positions, normals, uvs and indices.

      const auto& positions_buffer_view = buffer_views[positions_buffer_view_index];
      const auto& normals_buffer_view = buffer_views[normals_buffer_view_index];
      const auto& uvs_buffer_view = buffer_views[uvs_buffer_view_index];
      const auto& tangents_buffer_view = buffer_views[tangents_buffer_view_index];
      const auto& indices_buffer_view = buffer_views[indices_buffer_view_index];

      // [NOTE] KAJ 2024-03-20 : Here we get the positions data.

      const auto positions_buffer_index = positions_buffer_view["buffer"].get<std::size_t>();
      const auto positions_byte_offset = positions_buffer_view.contains("byteOffset") ? positions_buffer_view["byteOffset"].get<std::size_t>() : 0u;
      const auto positions_byte_length = positions_buffer_view["byteLength"].get<std::size_t>();

      const auto& positions_buffer = _decode_buffer(positions_buffer_index, path.parent_path(), decoded_buffers, buffers[positions_buffer_index]);

      const auto* positions_data = reinterpret_cast<const math::vector3*>(positions_buffer.data() + positions_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the normals data.

      const auto normals_buffer_index = normals_buffer_view["buffer"].get<std::size_t>();
      const auto normals_byte_offset = normals_buffer_view.contains("byteOffset") ? normals_buffer_view["byteOffset"].get<std::size_t>() : 0u;
      const auto normals_byte_length = normals_buffer_view["byteLength"].get<std::size_t>();

      const auto& normals_buffer = _decode_buffer(normals_buffer_index, path.parent_path(), decoded_buffers, buffers[normals_buffer_index]);

      const auto* normals_data = reinterpret_cast<const math::vector3*>(normals_buffer.data() + normals_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the uvs data.

      const auto uvs_buffer_index = uvs_buffer_view["buffer"].get<std::size_t>();
      const auto uvs_byte_offset = uvs_buffer_view.contains("byteOffset") ? uvs_buffer_view["byteOffset"].get<std::size_t>() : 0u;
      const auto uvs_byte_length = uvs_buffer_view["byteLength"].get<std::size_t>();

      const auto& uvs_buffer = _decode_buffer(uvs_buffer_index, path.parent_path(), decoded_buffers, buffers[uvs_buffer_index]);

      const auto* uvs_data = reinterpret_cast<const math::vector2*>(uvs_buffer.data() + uvs_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the tangents data.

      const auto tangents_buffer_index = tangents_buffer_view["buffer"].get<std::size_t>();
      const auto tangents_byte_offset = tangents_buffer_view.contains("byteOffset") ? tangents_buffer_view["byteOffset"].get<std::size_t>() : 0u;
      const auto tangents_byte_length = tangents_buffer_view["byteLength"].get<std::size_t>();

      const auto& tangents_buffer = _decode_buffer(tangents_buffer_index, path.parent_path(), decoded_buffers, buffers[tangents_buffer_index]);

      const auto* tangents_data = reinterpret_cast<const math::vector4*>(tangents_buffer.data() + tangents_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the indices data.

      const auto indices_buffer_index = indices_buffer_view["buffer"].get<std::size_t>();
      const auto indices_byte_offset = indices_buffer_view.contains("byteOffset") ? indices_buffer_view["byteOffset"].get<std::size_t>() : 0u;
      const auto indices_byte_length = indices_buffer_view["byteLength"].get<std::size_t>();

      const auto& indices_buffer = _decode_buffer(indices_buffer_index, path.parent_path(), decoded_buffers, buffers[indices_buffer_index]);

      const auto* indices_data = reinterpret_cast<const std::uint8_t*>(indices_buffer.data() + indices_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we add the positions, normals and uvs to the vertices.

      const auto vertices_count = data.vertices.size();

      data.vertices.reserve(vertices_count + positions_count);

      for (auto i = 0; i < positions_count; ++i) {
        const auto& position = transform * math::vector4{positions_data[i]};
        const auto& normal = transform * math::vector4{normals_data[i]};
        const auto& tangent = transform * math::vector4{tangents_data[i]};
        const auto& uv = uvs_data[i];

        data.vertices.push_back(models::vertex3d{position, normal, tangent, uv});
      }

      data.indices.reserve(data.indices.size() + indices_count);
      
      // [NOTE] KAJ 2024-03-20 : We need to add the current vertices_count since we pack all vertices into one buffer
      if (indices_accessor["componentType"].get<std::size_t>() == component_type::unsigned_short.id) {
        for (auto i = 0u; i < indices_count; ++i) {
          data.indices.push_back(vertices_count + static_cast<std::uint32_t>(_parse_index<std::uint16_t>(indices_data, i)));
        }
      } else {
        for (auto i = 0u; i < indices_count; ++i) {
          data.indices.push_back(vertices_count + _parse_index<std::uint32_t>(indices_data, i));
        }
      }

      submesh.index_count = static_cast<std::uint32_t>(indices_count);

      data.submeshes.push_back(submesh);
    }
  }

  return data;
}

} // namespace sbx::models
