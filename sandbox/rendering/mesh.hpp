#ifndef SBX_RENDERIN_MODEL_HPP_
#define SBX_RENDERIN_MODEL_HPP_

#include <string>
#include <vector>

#include <types/primitives.hpp>
#include <types/vector.hpp>

namespace sbx {

struct mesh_vertex {
  vector3 position;
  vector3 normal;
  vector2 uv;
};

class mesh {

public:

  mesh(const std::string& path);
  ~mesh() = default;

  const std::vector<mesh_vertex>& vertices() const;
  const std::vector<uint32>& indices() const;

private:

  // [TODO] KAJ 2021-11-12 12:21 - Clear up mesh loading.
  void _load(const std::string& path);

  std::vector<mesh_vertex> _vertices{};
  std::vector<uint32> _indices{};
  // std::vector<texture> _textures{};

}; // class model

} // namespace sbx

#endif // SBX_RENDERIN_MODEL_HPP_
