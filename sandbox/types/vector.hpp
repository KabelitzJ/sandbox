#ifndef SBX_TYPES_VECTOR_HPP_
#define SBX_TYPES_VECTOR_HPP_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#include "primitives.hpp"

namespace sbx {

// [TODO] KAJ 2021-11-11 20:19 - Find a way to fully hide glm. Maybe write own math library?

using vector2 = glm::vec2;
using vector3 = glm::vec3;
using vector4 = glm::vec4;

inline constexpr auto vector3_zero  = vector3(0.0f, 0.0f, 0.0f);
inline constexpr auto vector3_up    = vector3(0.0f, 1.0f, 0.0f); 
inline constexpr auto vector3_down  = vector3(0.0f, -1.0f, 0.0f);
inline constexpr auto vector3_right = vector3(1.0f, 0.0f, 0.0f);
inline constexpr auto vector3_left  = vector3(-1.0f, 0.0f, 0.0f);
inline constexpr auto vector3_backward = vector3(0.0f, 0.0f, 1.0f);
inline constexpr auto vector3_forward = vector3(0.0f, 0.0f, -1.0f);

float32 to_radians(float32 degrees);

float32 to_degrees(float32 radians);

template<typename Type>
const float32* value_ptr(const Type& type) {
  return glm::value_ptr(type);
}

vector3 cross(const vector3& lhs, const vector3& rhs);

vector3 normalize(const vector3& vector);

} // namespace sbx

#endif // SBX_TYPES_VECTOR_HPP_
