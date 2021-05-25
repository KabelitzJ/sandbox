#ifndef SBX_CORE_MESH_HPP_
#define SBX_CORE_MESH_HPP_

#include <filesystem>
#include <vector>

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.hpp"

namespace sbx {

class mesh {

public:
  mesh(const std::filesystem::path& path);
  ~mesh();

  void draw(/*const shader& shader*/);

private:
  struct vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
  };  

  void _load_from_obj(const std::filesystem::path& path);
  void _initialize();

  GLuint _vao;
  GLuint _vbo;
  GLuint _ebo;

  std::vector<vertex> _vertices;
  std::vector<GLuint> _indices;

}; // class mesh

} // namespace sbx

#endif // SBX_CORE_MESH_HPP_
