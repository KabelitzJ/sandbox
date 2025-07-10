#ifndef LIBSBX_ANIMATIONS_SKELETON_HPP_
#define LIBSBX_ANIMATIONS_SKELETON_HPP_

#include <string>
#include <cstdint>

#include <libsbx/math/matrix4x4.hpp>

namespace sbx::animations {

class skeleton {

public:

  struct bone {
    std::string name;
    std::uint32_t parent_id;
    math::matrix4x4 inverse_bind_matrix;
  }; // struct bone

  inline static constexpr auto max_bones = std::uint32_t{64u};

  skeleton() = default;

  // auto name() const -> const std::string& {
  //   return _name;
  // }

  auto add_bone(const bone& bone) -> void {
    _bones.push_back(bone);
  }

  auto bones() const -> const std::vector<bone>& {
    return _bones;
  }

private:

  // std::string _name;
  std::vector<bone> _bones;

}; // class skeleton

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_SKELETON_HPP_
