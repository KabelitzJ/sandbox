#ifndef SBX_RENDERING_MESH_HPP_
#define SBX_RENDERING_MESH_HPP_

#include <vector>

#include <types/primitives.hpp>
#include <types/vector.hpp>

namespace sbx {

struct mesh_vertex {
  vector3 position;
  vector3 normal;
  vector2 uv;
};

struct mesh {
  std::vector<mesh_vertex> vertices;
  std::vector<uint32> indices;
}; // struct mesh

} // namespace sbx

#endif // SBX_RENDERING_MESH_HPP_
