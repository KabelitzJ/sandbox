#ifndef LIBSBX_ANIMATIONS_SKELETON_HPP_
#define LIBSBX_ANIMATIONS_SKELETON_HPP_

#include <string>
#include <cstdint>
#include <unordered_map>
#include <cmath>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/hashed_string.hpp>

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

  auto set_inverse_root_transform(const math::matrix4x4& inverse_root_transform) -> void {
    _inverse_root_transform = inverse_root_transform;
  }

  auto bones() const -> const std::vector<bone>& {
    return _bones;
  }

  auto evaluate_pose(const animation& animation, std::float_t time) const -> std::vector<math::matrix4x4> {
    EASY_FUNCTION();
    SBX_SCOPED_TIMER("skeleton::evaluate_pose");

    auto final_bones = std::vector<math::matrix4x4>{};
    final_bones.resize(_bones.size(), math::matrix4x4::identity);

    auto global_transforms = std::vector<math::matrix4x4>{};
    global_transforms.resize(_bones.size(), math::matrix4x4::identity);

    for (std::uint32_t bone_id = 0; bone_id < _bones.size(); ++bone_id) {
      const auto& bone = _bones[bone_id];
      const auto& bone_name = _bone_ids_to_names[bone_id];

      math::matrix4x4 local_transform = bone.local_bind_matrix;

      const auto& track_map = animation.track_map;

      EASY_BLOCK("skeleton::find_track");

      auto it = track_map.find(bone_name);

      EASY_END_BLOCK;

      // Sample animation track if present
      if (it != track_map.cend()) {
        EASY_BLOCK("skeleton::sample_track");
        const auto& track = it->second;

        EASY_BLOCK("skeleton::sample_position_rotation_scale");

        const auto& position = track.position_spline.sample(time);
        const auto& rotation = math::quaternion::normalized(track.rotation_spline.sample(time));
        const auto& scale = track.scale_spline.sample(time);

        EASY_END_BLOCK;

        EASY_BLOCK("skeleton::calculate_local_transform");

        const auto translation_matrix = math::matrix4x4::translated(math::matrix4x4::identity, position);
        const auto rotation_matrix = rotation.to_matrix();
        const auto scale_matrix = math::matrix4x4::scaled(math::matrix4x4::identity, scale);

        EASY_END_BLOCK;

        local_transform = translation_matrix * rotation_matrix * scale_matrix;
        EASY_END_BLOCK;
      }

      EASY_BLOCK("skeleton::local_transform");

      const auto global_transform = (bone.parent_id != skeleton::bone::null) ? global_transforms[bone.parent_id] * local_transform : local_transform;

      final_bones[bone_id] = _inverse_root_transform * global_transform * bone.inverse_bind_matrix;

      global_transforms[bone_id] = std::move(global_transform);

      EASY_END_BLOCK;
    }

    return final_bones;
  }

  auto bone_count() const -> std::uint32_t {
    return _bones.size();
  }

  auto name_for_bone(std::size_t i) -> std::string {
    return _bone_ids_to_names[i].str();
  }

private:

  std::vector<bone> _bones;
  std::vector<utility::hashed_string> _bone_ids_to_names;
  std::unordered_map<utility::hashed_string, std::uint32_t> _bone_names;
  
  math::matrix4x4 _inverse_root_transform;

}; // class skeleton

} // namespace sbx::animation

#endif // LIBSBX_ANIMATIONS_SKELETON_HPP_
