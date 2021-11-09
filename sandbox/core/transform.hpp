#ifndef SBX_CORE_TRANSFORM_HPP_
#define SBX_CORE_TRANSFORM_HPP_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace sbx {

struct transform {
  glm::vec3 position{};
  glm::quat rotation{};
  glm::vec3 scale{};
}; // struct transform

} // namespace sbx

#endif // SBX_CORE_TRANSFORM_HPP_
