#ifndef SBX_RENDERIN_MODEL_HPP_
#define SBX_RENDERIN_MODEL_HPP_

#include <string>
#include <vector>

#include "mesh.hpp"

namespace sbx {

class model {

public:

  model(const std::string& path);
  ~model() = default;

  const std::vector<mesh>& meshes() const;

private:

  void _load(const std::string& path);

  std::vector<mesh> _meshes{};

}; // class model

} // namespace sbx

#endif // SBX_RENDERIN_MODEL_HPP_
