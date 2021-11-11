#ifndef SBX_TYPES_VECTOR_HPP_
#define SBX_TYPES_VECTOR_HPP_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sbx {

// [TODO] KAJ 2021-11-11 20:19 - Find a way to fully hide glm. Maybe write own math library?

using vector2 = glm::vec2;
using vector3 = glm::vec3;
using vector4 = glm::vec4;

} // namespace sbx

#endif // SBX_TYPES_VECTOR_HPP_
