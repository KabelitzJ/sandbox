#include "mesh.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <cassert>
#include <unordered_map>

#include <types/vector.hpp>

template<>
struct std::hash<sbx::mesh_vertex> {
  std::size_t operator()(const sbx::mesh_vertex& mesh_vertex) const {
    return std::hash<sbx::vector3>{}(mesh_vertex.position) ^
           std::hash<sbx::vector3>{}(mesh_vertex.normal) ^
           std::hash<sbx::vector2>{}(mesh_vertex.uv);
  }
}; // struct std::hash

namespace sbx {

bool operator==(const mesh_vertex& lhs, const mesh_vertex& rhs) {
  return lhs.position == rhs.position &&
         lhs.normal == rhs.normal &&
         lhs.uv == rhs.uv;
}

mesh::mesh(const std::string& path)
: _vertices{},
  _indices{} {
  _load(path);
}

const std::vector<mesh_vertex>& mesh::vertices() const {
  return _vertices;
}

const std::vector<uint32_t>& mesh::indices() const {
  return _indices;
}

void mesh::_load(const std::string& path) {
  auto file = std::ifstream(path, std::ios::in | std::ios::binary);

  assert(file.is_open()); // File could not be opened

  auto line = std::string{};
  auto tokens = std::stringstream{};

  auto vertices = std::vector<vector3>{};
  auto normals = std::vector<vector3>{};
  auto uvs = std::vector<vector2>{};

  auto vertex_indices = std::vector<uint32>{};
  auto normal_indices = std::vector<uint32>{};
  auto uv_indices = std::vector<uint32>{};

  while (std::getline(file, line)) {
    tokens << line;

    auto id = std::string{};

    tokens >> id;

    if (id == "v") {
      auto vertex = vector3{};
      tokens >> vertex.x >> vertex.y >> vertex.z;
      vertices.push_back(vertex);
    } else if (id == "vn") {
      auto normal = vector3{};
      tokens >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
    } else if (id == "vt") {
      auto uv = vector2{};
      tokens >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (id == "f") {
      auto temp_indices = std::array<std::string, 3>{};
      tokens >> temp_indices[0] >> temp_indices[1] >> temp_indices[2];

      for (const auto& temp_index : temp_indices) {
        auto temps = std::array<uint32, 3>{};
        const auto matches = sscanf(temp_index.c_str(), "%u/%u/%u", &temps[0], &temps[1], &temps[2]);

        assert(matches == 3); // Invalid face formatS

        vertex_indices.push_back(temps[0]);
        normal_indices.push_back(temps[1]);
        uv_indices.push_back(temps[2]);
      }
    }

    tokens.str(std::string{});
    tokens.clear();
  }

  auto indices_per_vertex = std::unordered_map<mesh_vertex, uint32>{};

  for (auto i = std::size_t{0u}; i < vertex_indices.size(); ++i) {
    auto vertex_index = vertex_indices[i];
    auto normal_index = normal_indices[i];
    auto uv_index = uv_indices[i];

    auto vertex = mesh_vertex{
      vertices[vertex_index - 1],
      normals[normal_index - 1],
      uvs[uv_index - 1]
    };

    if (indices_per_vertex.find(vertex) == indices_per_vertex.cend()) {
      indices_per_vertex[vertex] = static_cast<uint32>(_vertices.size());
      _vertices.push_back(vertex);
    }

    _indices.push_back(indices_per_vertex[vertex]);
  }

}

} // namespace sbx
