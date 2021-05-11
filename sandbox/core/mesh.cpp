#include "mesh.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <array>

namespace sbx {

mesh::mesh(const std::filesystem::path& path) : _vao(0), _vbo(0), _ebo(0), _vertices(), _indices() {
  _load_from_obj(path);
  _initialize();
}

mesh::~mesh() {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

void mesh::draw(const shader& shader) {
  shader.bind();

  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

  shader.unbind();
}

void mesh::_load_from_obj(const std::filesystem::path& path) {
  std::ifstream file_stream(path);

  if (!file_stream.is_open()) {
    std::stringstream ss;

    ss << "[Error] Could not open model file: '" << path << "'!\n";

    throw std::runtime_error(ss.str());
  }

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

          _vertices.push_back(temp_vertex);
          _indices.push_back(index);
        } else {
          _indices.push_back(itr->second);
        }
      }
    }
  }
}

void mesh::_initialize() {
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(0));
  glEnableVertexAttribArray(0);
  // uv
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, uv)));
  glEnableVertexAttribArray(1);
  // normal
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, normal)));
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), _indices.data(), GL_STATIC_DRAW); 

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace sbx
