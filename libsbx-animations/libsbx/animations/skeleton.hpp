#ifndef LIBSBX_ANIMATIONS_SKELETON_HPP_
#define LIBSBX_ANIMATIONS_SKELETON_HPP_

#include <string>
#include <cstdint>
#include <unordered_map>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/matrix4x4.hpp>

namespace sbx::animations {

class skeleton {

public:

  struct bone {
    inline static constexpr auto null = std::uint32_t{0xFFFFFFFF};

    std::uint32_t parent_id;
    math::matrix4x4 inverse_bind_matrix;
  }; // struct bone

  inline static constexpr auto max_bones = std::uint32_t{64u};

  skeleton() {
    _bones.reserve(32u);
    _bone_names.reserve(32);
  }

  auto bone_index(const std::string& name) const -> std::uint32_t {
    auto entry = _bone_names.find(name);

    return (entry != _bone_names.cend()) ? entry->second : bone::null;
  }

  auto add_bone(const std::string& name, const bone& bone) -> void {
    _bone_names.emplace(name, static_cast<std::uint32_t>(_bones.size()));
    _bones.push_back(bone);
  }

  auto bones() const -> const std::vector<bone>& {
    return _bones;
  }

private:

  std::vector<bone> _bones;
  std::unordered_map<std::string, std::uint32_t> _bone_names;

}; // class skeleton

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_SKELETON_HPP_
