#ifndef SBX_CORE_MESH_LOADER_HPP_
#define SBX_CORE_MESH_LOADER_HPP_

#include <vector>
#include <filesystem>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace sbx {

struct vertex {
  glm::vec3 position;
  glm::vec2 uv;
  glm::vec3 normal;
};

struct basic_model {
  std::vector<vertex> vertices;
  std::vector<GLuint> indices;
};

basic_model load_basic_model(const std::filesystem::path& path);

} // namespace sbx

#endif // SBX_CORE_MESH_LOADER_HPP_
