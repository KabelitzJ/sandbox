#ifndef SBX_CORE_MATERIAL_HPP_
#define SBX_CORE_MATERIAL_HPP_

#include "texture.hpp"

namespace sbx {

struct material {
  texture* diffuse;
  texture* specular;
  float shininess;
}; // struct material

} // namespace sbx

#endif // SBX_CORE_MATERIAL_HPP_