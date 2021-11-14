#ifndef SBX_TYPES_TRANSFORM_HPP_
#define SBX_TYPES_TRANSFORM_HPP_

#include <glm/gtc/quaternion.hpp>

#include <types/vector.hpp>
#include <types/quaternion.hpp>

namespace sbx {

/**
 * @brief Defines a position, an orientation and a scale in 3D space.
 */
struct transform {
  vector3 position{};
  quaternion rotation{};
  vector3 scale{};
}; // struct transform

} // namespace sbx

#endif // SBX_TYPES_TRANSFORM_HPP_
