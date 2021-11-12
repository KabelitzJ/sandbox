#include "model.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <cassert>

#include <types/primitives.hpp>
#include <types/vector.hpp>

#include "mesh.hpp"

namespace sbx {

model::model(const std::string& path)
: _meshes{} {
  _load(path);
}

const std::vector<mesh>& model::meshes() const {
  return _meshes;
}

void model::_load(const std::string& path) {
  auto file = std::ifstream(path, std::ios::in | std::ios::binary);

  assert(file.is_open()); // File could not be opened

  auto line = std::string{};
  auto tokens = std::stringstream{};

  auto vertices = std::vector<vector3>{};
  auto normals = std::vector<vector3>{};
  auto uvs = std::vector<vector2>{};

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
      auto entries = std::array<std::string, 3>{};

      tokens >> entries[0] >> entries[1] >> entries[2];

      for (const auto entry : entries) {
        auto stream = std::stringstream{entry};
        auto index = std::string{};

        while (std::getline(stream, index, '/')) {
          auto i = std::stoi(index);

          auto vertex = vertices[i];
          auto normal = normals[i];
          auto uv = uvs[i];
        
          // [TODO] KAJ 2021-11-12 08:47 - Build mesh here
        }
      }
    }

    tokens.str(std::string{});
    tokens.clear();
  }
}

} // namespace sbx 
