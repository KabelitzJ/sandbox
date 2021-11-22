#include "quaternion.hpp"

#include <glm/gtx/quaternion.hpp>

namespace sbx {

quaternion rotate(const quaternion& q, const vector3& v) {
  return glm::rotate(q, v);
}

} // namespace sbx
