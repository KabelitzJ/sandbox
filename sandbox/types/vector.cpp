#include "vector.hpp"

namespace sbx {

float32 to_radians(float32 degrees) {
  return glm::radians(degrees);
}

float32 to_degrees(float32 radians) {
  return glm::degrees(radians);
}

} // namespace sbx
