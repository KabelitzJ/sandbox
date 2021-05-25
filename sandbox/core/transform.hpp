#ifndef SBX_CORE_TRANSFORM_HPP_
#define SBX_CORE_TRANSFORM_HPP_

#include <glm/vec3.hpp>

namespace sbx {

struct transform {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
}; // struct transform

} // namespace sbx

#endif // SBX_CORE_TRANSFORM_HPP_
