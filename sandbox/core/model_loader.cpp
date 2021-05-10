#include "model_loader.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace sbx {

basic_model load_basic_model(const std::filesystem::path& path) {
  std::ifstream file_stream(path);

  if (!file_stream.is_open()) {
    std::stringstream ss;

    ss << "[Error] Could not open model file: '" << path << "'!\n";

    throw std::runtime_error(ss.str());
  }

  basic_model model;
  std::string line;
  std::istringstream line_stream;

  std::vector<glm::vec3> temp_positions;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;
  std::unordered_map<std::string, GLuint> vertex_map;
  GLuint next_index = 0;

  while (std::getline(file_stream, line)) {
    line_stream.str(line);
    line_stream.clear();

    std::string token;

    line_stream >> token;

    if (token == "v") {
      glm::vec3 position;

      line_stream >> position.x >> position.y >> position.z;

      temp_positions.push_back(position);
    } else if (token == "vn") {
      glm::vec3 normal;

      line_stream >> normal.x >> normal.y >> normal.z;

      temp_normals.push_back(normal);
    } else if (token == "vt") {
      glm::vec2 uv;

      line_stream >> uv.s >> uv.t;

      temp_uvs.push_back(uv);
    } else if (token == "f") {
      std::array<std::string, 3> vertex_keys;

      line_stream >> vertex_keys[0] >> vertex_keys[1] >> vertex_keys[2];

      for (const std::string& key : vertex_keys) {
        const auto itr = vertex_map.find(key);

        if (itr == vertex_map.end()) {
          GLuint position_index;
          GLuint uv_index;
          GLuint normal_index;

          unsigned int matches = sscanf(key.c_str(), "%u/%u/%u", &position_index, &uv_index, &normal_index);

          if (matches != 3) {
            std::stringstream ss;

            ss << "[Error] Could not parse model file: '" << path << "'!\n";

            throw std::runtime_error(ss.str());
          }

          vertex temp_vertex;

          temp_vertex.position = temp_positions[position_index - 1];
          temp_vertex.uv = temp_uvs[uv_index - 1];
          temp_vertex.normal = temp_normals[normal_index - 1];

          const GLuint index = next_index++;

          vertex_map.emplace(key, index);

          model.vertices.push_back(temp_vertex);
          model.indices.push_back(index);
        } else {
          model.indices.push_back(itr->second);
        }
      }
    }
  }

  return model;
}

} // namespace sbx
