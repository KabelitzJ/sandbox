#ifndef SBX_CORE_MESH_HPP_
#define SBX_CORE_MESH_HPP_

#include <filesystem>
#include <vector>

#include <glad/glad.h>

#include "shader.hpp"
#include "base_resource.hpp"

namespace sbx {

class mesh : public base_resource {

public:
  mesh(const std::filesystem::path& path);
  ~mesh();

  void draw(/*const shader& shader*/);

private:
  mesh();

  base_resource_data* _load(const std::filesystem::path& path) override;
  void _initialize(base_resource_data* resource_data) override;

  GLuint _vao;
  GLuint _vbo;
  GLuint _ebo;

  unsigned int _indices_count;

}; // class mesh

} // namespace sbx

#endif // SBX_CORE_MESH_HPP_
