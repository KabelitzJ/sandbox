#ifndef SBX_CORE_CAMERA_HPP_
#define SBX_CORE_CAMERA_HPP_

#include <types/vector.hpp>
#include <types/matrix.hpp>

namespace sbx {

struct camera {
  bool is_main{};
  matrix4x4 view_matrix{};
  matrix4x4 projection_matrix{};
}; // struct camera

} // namespace sbx

#endif // SBX_CORE_CAMERA_HPP_
