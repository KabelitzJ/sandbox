#ifndef SBX_CORE_TRANSFORM_HPP_
#define SBX_CORE_TRANSFORM_HPP_

#include <glm/gtc/quaternion.hpp>

#include <types/vector.hpp>

namespace sbx {

struct transform {
  vector3 position{};
  glm::quat rotation{};
  vector3 scale{};
}; // struct transform

} // namespace sbx

#endif // SBX_CORE_TRANSFORM_HPP_
