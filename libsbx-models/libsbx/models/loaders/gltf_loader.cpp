#include <libsbx/models/loaders/gltf_loader.hpp>

#include <filesystem>
#include <fstream>
#include <bit>

#include <fmt/format.h>

#include <libbase64.h>

#include <nlohmann/json.hpp>

#include <libsbx/math/vector2.hpp>  
#include <libsbx/math/vector3.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/quaternion.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

static auto _decode_buffer(std::size_t index, std::unordered_map<std::size_t, std::vector<std::uint8_t>>& buffers, const nlohmann::json& json) -> const std::vector<std::uint8_t>& {
  if (buffers.contains(index)) {
    return buffers.at(index);
  }

  const auto byte_length = json["byteLength"].get<std::size_t>();
  const auto uri = json["uri"].get<std::string>();

  auto start = uri.find("data:application/octet-stream;base64,");

  if (start == std::string::npos) {
    throw std::runtime_error{fmt::format("Invalid base64 URI: {}", uri.substr(0, uri.find(',')))};
  }

  const auto buffer_base64 = uri.substr(start + 37u);

  auto buffer = std::vector<std::uint8_t>{};
  buffer.resize(byte_length);

  auto buffer_size_out = std::size_t{};

  if (!base64_decode(buffer_base64.data(), buffer_base64.size(), reinterpret_cast<char*>(buffer.data()), &buffer_size_out, 0)) {
    throw std::runtime_error{"Failed to decode base64 buffer"};
  }

  if (buffer_size_out != buffer.size()) {
    throw std::runtime_error{fmt::format("Decoded buffer size does not match expected size: {} != {}", buffer_size_out, buffer.size())};
  }

  return (buffers[index] = buffer);
}

auto gltf_loader::load(const std::filesystem::path& path) -> mesh_data {
  auto data = mesh_data{};

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
    const auto mesh_index = node["mesh"].get<std::size_t>();

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

    const auto& mesh = meshes[mesh_index];

    const auto mesh_name = mesh["name"].get<std::string>();
    auto submesh = graphics::submesh{};

    core::logger::debug("Loading submesh '{}'", mesh_name);

    submesh.index_offset = static_cast<std::uint32_t>(data.indices.size());
    // submesh.vertex_offset = static_cast<std::uint32_t>(data.vertices.size()); 

    // [NOTE] KAJ 2023-11-22 : This is a offset into the vertex buffer. We dont want to use this.
    submesh.vertex_offset = 0u;

    const auto& primitives = mesh["primitives"];
    
    for (const auto& primitive : primitives) {
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

      if (!primitive.contains("indices")) {
        throw std::runtime_error{fmt::format("Mesh '{}' in file '{}' does not contain the attribute 'indices'", mesh_name, path.string())};
      }

      const auto positions_index = primitive["attributes"]["POSITION"].get<std::size_t>();
      const auto normals_index = primitive["attributes"]["NORMAL"].get<std::size_t>();
      const auto uvs_index = primitive["attributes"]["TEXCOORD_0"].get<std::size_t>();
      const auto indices_index = primitive["indices"].get<std::size_t>();

      // [NOTE] KAJ 2024-03-20 : Here we get the accessors for the positions, normals, uvs and indices.

      const auto& positions_accessor = accessors[positions_index];
      const auto& normals_accessor = accessors[normals_index];
      const auto& uvs_accessor = accessors[uvs_index];
      const auto& indices_accessor = accessors[indices_index];

      // [NOTE] KAJ 2024-03-20 : We need to check if the accessors have the correct component type and type.

      if (positions_accessor["componentType"].get<std::size_t>() != 5126 || positions_accessor["type"].get<std::string>() != "VEC3") {
        throw std::runtime_error{"Invalid component type or type for positions accessor"};
      }

      if (normals_accessor["componentType"].get<std::size_t>() != 5126 || normals_accessor["type"].get<std::string>() != "VEC3") {
        throw std::runtime_error{"Invalid component type or type for normals accessor"};
      }

      if (uvs_accessor["componentType"].get<std::size_t>() != 5126 || uvs_accessor["type"].get<std::string>() != "VEC2") {
        throw std::runtime_error{"Invalid component type or type for uvs accessor"};
      }

      // [TODO] KAJ 2024-03-20 : We should enable different component types for the indices accessor. For now we only support std::uint16_t.

      if (indices_accessor["componentType"].get<std::size_t>() != 5123 || indices_accessor["type"].get<std::string>() != "SCALAR") {
        throw std::runtime_error{"Invalid component type or type for indices accessor"};
      }

      const auto positions_count = positions_accessor["count"].get<std::size_t>();
      const auto normals_count = normals_accessor["count"].get<std::size_t>();
      const auto uvs_count = uvs_accessor["count"].get<std::size_t>();
      const auto indices_count = indices_accessor["count"].get<std::size_t>();

      if (positions_count != normals_count || positions_count != uvs_count) {
        throw std::runtime_error{"Mismatching counts for positions, normals and uvs accessors"};
      }

      const auto positions_buffer_view_index = positions_accessor["bufferView"].get<std::size_t>();
      const auto normals_buffer_view_index = normals_accessor["bufferView"].get<std::size_t>();
      const auto uvs_buffer_view_index = uvs_accessor["bufferView"].get<std::size_t>();
      const auto indices_buffer_view_index = indices_accessor["bufferView"].get<std::size_t>();

      // [NOTE] KAJ 2024-03-20 : Here we get the buffer views for the positions, normals, uvs and indices.

      const auto& positions_buffer_view = buffer_views[positions_buffer_view_index];
      const auto& normals_buffer_view = buffer_views[normals_buffer_view_index];
      const auto& uvs_buffer_view = buffer_views[uvs_buffer_view_index];
      const auto& indices_buffer_view = buffer_views[indices_buffer_view_index];

      // [NOTE] KAJ 2024-03-20 : Here we get the positions data.

      const auto positions_buffer_index = positions_buffer_view["buffer"].get<std::size_t>();
      const auto positions_byte_offset = positions_buffer_view["byteOffset"].get<std::size_t>();
      const auto positions_byte_length = positions_buffer_view["byteLength"].get<std::size_t>();

      const auto& positions_buffer = _decode_buffer(positions_buffer_index, decoded_buffers, buffers[positions_buffer_index]);

      const auto* positions_data = reinterpret_cast<const math::vector3*>(positions_buffer.data() + positions_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the normals data.

      const auto normals_buffer_index = normals_buffer_view["buffer"].get<std::size_t>();
      const auto normals_byte_offset = normals_buffer_view["byteOffset"].get<std::size_t>();
      const auto normals_byte_length = normals_buffer_view["byteLength"].get<std::size_t>();

      const auto& normals_buffer = _decode_buffer(normals_buffer_index, decoded_buffers, buffers[normals_buffer_index]);

      const auto* normals_data = reinterpret_cast<const math::vector3*>(normals_buffer.data() + normals_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the uvs data.

      const auto uvs_buffer_index = uvs_buffer_view["buffer"].get<std::size_t>();
      const auto uvs_byte_offset = uvs_buffer_view["byteOffset"].get<std::size_t>();
      const auto uvs_byte_length = uvs_buffer_view["byteLength"].get<std::size_t>();

      const auto& uvs_buffer = _decode_buffer(uvs_buffer_index, decoded_buffers, buffers[uvs_buffer_index]);

      const auto* uvs_data = reinterpret_cast<const math::vector2*>(uvs_buffer.data() + uvs_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we get the indices data.

      const auto indices_buffer_index = indices_buffer_view["buffer"].get<std::size_t>();
      const auto indices_byte_offset = indices_buffer_view["byteOffset"].get<std::size_t>();
      const auto indices_byte_length = indices_buffer_view["byteLength"].get<std::size_t>();

      const auto& indices_buffer = _decode_buffer(indices_buffer_index, decoded_buffers, buffers[indices_buffer_index]);

      const auto* indices_data = reinterpret_cast<const std::uint16_t*>(indices_buffer.data() + indices_byte_offset);

      // [NOTE] KAJ 2024-03-20 : Here we add the positions, normals and uvs to the vertices.

      // for (auto i = 0u; i < positions_count; ++i) {
      //   const auto position = transform * math::vector4{positions_data[i * 3u + 0u], positions_data[i * 3u + 1u], positions_data[i * 3u + 2u], 0.0f};
      //   const auto normal = transform * math::vector4{normals_data[i * 3u + 0u], normals_data[i * 3u + 1u], normals_data[i * 3u + 2u], 0.0f};
      //   const auto uv = math::vector2{uvs_data[i * 2u + 0u], uvs_data[i * 2u + 1u]};

      //   data.vertices.push_back(models::vertex3d{position, normal, uv});
      // }

      data.vertices.reserve(data.vertices.size() + positions_count);

      for (auto i = 0; i < positions_count; ++i) {
        const auto& position = transform * math::vector4{positions_data[i]};
        const auto& normal = transform * math::vector4{normals_data[i]};
        const auto& uv = uvs_data[i];

        data.vertices.push_back(models::vertex3d{position, normal, uv});
      }

      data.indices.reserve(data.indices.size() + indices_count);

      for (auto i = 0u; i < indices_count; ++i) {
        data.indices.push_back(static_cast<std::uint32_t>(indices_data[i]));
      }

      submesh.index_count = static_cast<std::uint32_t>(indices_count);

      data.submeshes.push_back(submesh);
    }

    // break;
  }

  return data;
}

} // namespace sbx::models
