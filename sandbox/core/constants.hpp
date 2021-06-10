#ifndef SBX_CORE_CONSTANTS_HPP_
#define SBX_CORE_CONSTANTS_HPP_

#include <glm/vec3.hpp>

namespace sbx {

constexpr glm::vec3 VECTOR_UP(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 VECTOR_DOWN(0.0f, -1.0f, 0.0f);
constexpr glm::vec3 VECTOR_LEFT(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 VECTOR_RIGHT(-1.0f, 0.0f, 0.0f);
constexpr glm::vec3 VECTOR_FOREWARDS(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 VECTOR_BACKWARDS(0.0f, 0.0f, -1.0f);

} // namespace sbx

#endif // SBX_CORE_CONSTANTS_HPP_
