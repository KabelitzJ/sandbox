#ifndef SBX_PHYSICS_RIGIDBODY_HPP_
#define SBX_PHYSICS_RIGIDBODY_HPP_

#include <types/primitives.hpp>
#include <types/vector.hpp>

namespace sbx {

struct rigidbody {
  vector3 velocity{};
  float32 mass{};
  bool is_static{};
}; // struct rigidbody

} // namespace sbx

#endif // SBX_PHYSICS_RIGIDBODY_HPP_
