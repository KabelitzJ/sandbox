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

    utility::logger<"asd">::debug("{}", time);

    std::unordered_map<std::string, animation::bone_track> track_map;
    for (const auto& track : animation.tracks) {
      track_map[track.bone_name] = track;
    }

    std::function<void(std::uint32_t, const math::matrix4x4&)> evaluate_bone = [&](std::uint32_t bone_id, const math::matrix4x4& parent_transform) {
      const auto& bone = _bones[bone_id];

      // Get animation track
      const std::string& bone_name = _bone_ids_to_names[bone_id];
      const auto it = track_map.find(bone_name);
      math::matrix4x4 local_transform = math::matrix4x4::identity;

      if (it != track_map.end()) {
        const auto& track = it->second;
        const auto& keys = track.keyframes;

        // Find two surrounding keyframes
        std::size_t k1 = 0, k2 = 0;
        for (std::size_t i = 0; i + 1 < keys.size(); ++i) {
          if (keys[i].time <= time && time < keys[i + 1].time) {
            k1 = i;
            k2 = i + 1;
            break;
          }
        }

        const auto& a = keys[k1];
        const auto& b = keys[k2];
        const float t = (time - a.time) / (b.time - a.time);

        const auto position = math::vector3::lerp(a.position, b.position, t);
        const auto scale = math::vector3::lerp(a.scale, b.scale, t);
        const auto rotation = math::quaternion::slerp(a.rotation, b.rotation, t);

        const auto translation_matrix = math::matrix4x4::translated(math::matrix4x4::identity, position);
        const auto rotation_matrix = rotation.to_matrix();
        const auto scale_matrix = math::matrix4x4::scaled(math::matrix4x4::identity, scale);

        local_transform = translation_matrix * rotation_matrix * scale_matrix;
      }

      auto global_transform = parent_transform * local_transform;
      
      final_bones[bone_id] = global_transform * bone.inverse_bind_matrix;

      for (std::uint32_t child_id = 0; child_id < _bones.size(); ++child_id) {
        if (_bones[child_id].parent_id == bone_id) {
          evaluate_bone(child_id, global_transform);
        }
      }
    };

    // Start from roots (parent_id == max_bones)
    for (std::uint32_t i = 0; i < _bones.size(); ++i) {
      if (_bones[i].parent_id == skeleton::max_bones) {
        evaluate_bone(i, math::matrix4x4::identity);
      }
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
