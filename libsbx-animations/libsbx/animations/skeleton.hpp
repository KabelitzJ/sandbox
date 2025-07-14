#ifndef LIBSBX_ANIMATIONS_SKELETON_HPP_
#define LIBSBX_ANIMATIONS_SKELETON_HPP_

#include <string>
#include <cstdint>
#include <unordered_map>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/matrix4x4.hpp>

#include <libsbx/animations/animation.hpp>

namespace sbx::animations {

class skeleton {

public:

  struct bone {
    inline static constexpr auto null = std::uint32_t{0xFFFFFFFF};

    std::uint32_t parent_id;
    math::matrix4x4 local_bind_matrix;
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
    _bone_ids_to_names.push_back(name);
    _bones.push_back(bone);
  }

  auto bones() const -> const std::vector<bone>& {
    return _bones;
  }

  auto evaluate_pose(const animation& animation, std::float_t time) const -> std::vector<math::matrix4x4> {
    auto final_bones = std::vector<math::matrix4x4>{};
    final_bones.resize(_bones.size(), math::matrix4x4::identity);

    auto global_transforms = std::vector<math::matrix4x4>{};
    global_transforms.resize(_bones.size(), math::matrix4x4::identity);

    for (std::uint32_t bone_id = 0; bone_id < _bones.size(); ++bone_id) {
      const auto& bone = _bones[bone_id];
      const std::string& bone_name = _bone_ids_to_names[bone_id];

      utility::logger<"animations">::debug("name {}", bone_name);

      math::matrix4x4 local_transform = math::matrix4x4::identity;

      const auto& track_map = animation.track_map;

      // Sample animation track if present
      if (const auto it = track_map.find(bone_name); it != track_map.cend()) {
        const auto& track = it->second;

        utility::logger<"animations">::debug("  time {}", time);

        const auto& position = track.position_spline.sample(time);
        const auto& rotation = track.rotation_spline.sample(time);
        const auto& scale = track.scale_spline.sample(time); // NOTE: scale is NAN... FIX THIS

        utility::logger<"animations">::debug("  position {}", position);
        utility::logger<"animations">::debug("  rotation {}", rotation);
        utility::logger<"animations">::debug("  scale {}", scale);

        const auto translation_matrix = math::matrix4x4::translated(math::matrix4x4::identity, position);
        const auto rotation_matrix = rotation.to_matrix();
        const auto scale_matrix = math::matrix4x4::scaled(math::matrix4x4::identity, scale);

        local_transform = translation_matrix * rotation_matrix * scale_matrix;
      } else {
        local_transform = bone.local_bind_matrix;
      }

      // Compute global transform
      math::matrix4x4 parent_transform = (bone.parent_id != skeleton::bone::null) ? global_transforms[bone.parent_id] : math::matrix4x4::identity;

      const math::matrix4x4 global_transform = parent_transform * local_transform;

      global_transforms[bone_id] = global_transform;

      // Final bone matrix
      final_bones[bone_id] = global_transform * bone.inverse_bind_matrix;
    }

    return final_bones;
  }

private:

  std::vector<bone> _bones;
  std::vector<std::string> _bone_ids_to_names;
  std::unordered_map<std::string, std::uint32_t> _bone_names;

}; // class skeleton

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_SKELETON_HPP_
