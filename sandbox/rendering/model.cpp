#include "model.hpp"

namespace sbx {

model::model(const std::string& path)
: _meshes{} {
  _load(path);
}

void model::_load(const std::string& path) {
  static_cast<void>(path);
}

} // namespace sbx 
