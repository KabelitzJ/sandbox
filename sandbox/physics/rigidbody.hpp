#ifndef SBX_PHYSICS_RIGIDBODY_HPP_
#define SBX_PHYSICS_RIGIDBODY_HPP_

#include <glm/vec3.hpp>

#include <types/primitives.hpp>

namespace sbx {

struct rigidbody {
  glm::vec3 velocity{}
  float32 mass{};
}; // struct rigidbody

} // namespace sbx

#endif // SBX_PHYSICS_RIGIDBODY_HPP_
