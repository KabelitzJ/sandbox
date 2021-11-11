#ifndef SBX_CORE_TRANSFORM_HPP_
#define SBX_CORE_TRANSFORM_HPP_

#include <glm/gtc/quaternion.hpp>

#include <types/vector.hpp>

namespace sbx {

/**
 * @brief Defines a position and orientation in 3D space.
 */
struct transform {
  vector3 position{};
  glm::quat rotation{};
  vector3 scale{};
}; // struct transform

} // namespace sbx

#endif // SBX_CORE_TRANSFORM_HPP_
