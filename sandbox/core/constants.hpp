#ifndef SBX_CORE_CONSTANTS_HPP_
#define SBX_CORE_CONSTANTS_HPP_

#include <glm/vec3.hpp>

namespace sbx {

constexpr glm::vec3 ORIGIN{0.0f, 0.0f, 0.0f};
constexpr glm::vec3 UP{0.0f, 1.0f, 0.0f};
constexpr glm::vec3 DOWN{0.0f, -1.0f, 0.0f};
constexpr glm::vec3 LEFT{1.0f, 0.0f, 0.0f};
constexpr glm::vec3 RIGHT{-1.0f, 0.0f, 0.0f};
constexpr glm::vec3 FOREWARDS{0.0f, 0.0f, 1.0f};
constexpr glm::vec3 BACKWARDS{0.0f, 0.0f, -1.0f};

} // namespace sbx

#endif // SBX_CORE_CONSTANTS_HPP_
