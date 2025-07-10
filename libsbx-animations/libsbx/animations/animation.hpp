#ifndef LIBSBX_ANIMATIONS_ANIMATION_HPP_
#define LIBSBX_ANIMATIONS_ANIMATION_HPP_

#include <string>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <libsbx/utility/logger.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/quaternion.hpp>

namespace sbx::animations {

class animation {

public:

  struct keyframe {
    float time; // in seconds
    math::vector3 position;
    math::vector3 scale;
    math::quaternion rotation; // use quaternion for rotation
  };

  struct bone_track {
    std::string bone_name; // or bone_id
    std::vector<keyframe> keyframes;
  };

  std::string name;
  float duration = 0.0f;
  float ticks_per_second = 25.0f;
  std::vector<bone_track> tracks;

  auto track_for_bone(const std::string& bone_name) const -> const bone_track* {
    return nullptr;
  }

  static auto _convert_vec2(const aiVector2D& vector) -> math::vector2 {
    return math::vector2{vector.x, vector.y};
  }

  static auto _convert_vec3(const aiVector3D& vector) -> math::vector3 {
    return math::vector3{vector.x, vector.y, vector.z};
  }

  static auto _convert_mat4(const aiMatrix4x4& matrix) -> math::matrix4x4 {
    auto result = math::matrix4x4{};

    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
    result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
    result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
    result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;

    return result;
  }

  static auto load_animation_clip(const std::filesystem::path& path) -> animation {
    static const auto import_flags =
      aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
      aiProcess_Triangulate |             // Make sure we're triangles
      aiProcess_SortByPType |             // Split meshes by primitive type
      aiProcess_GenNormals |              // Make sure we have legit normals
      aiProcess_GenUVCoords |             // Convert UVs if required
      aiProcess_OptimizeMeshes |          // Batch draws where possible
      aiProcess_JoinIdenticalVertices |
      aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
      aiProcess_GlobalScale |             // e.g. convert cm to m for fbx import (and other formats where cm is native)
      aiProcess_ValidateDataStructure;    // Validation 

    auto importer = Assimp::Importer{};

    const auto* scene = importer.ReadFile(path.string(), import_flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      throw std::runtime_error{fmt::format("Error loading mesh '{}': {}", path.string(), importer.GetErrorString())};
    }

    auto clip = animation{};

    const aiAnimation* anim = scene->mAnimations[0]; // or all animations
    clip.name = anim->mName.C_Str();
    clip.duration = anim->mDuration > 0.0 ? static_cast<float>(anim->mDuration) : 150.0f;
    clip.ticks_per_second = anim->mTicksPerSecond > 0.0 ? static_cast<float>(anim->mTicksPerSecond) : 25.0f;

    for (unsigned i = 0; i < anim->mNumChannels; ++i) {
      const aiNodeAnim* channel = anim->mChannels[i];
      animation::bone_track track;
      track.bone_name = channel->mNodeName.C_Str();

      for (unsigned k = 0; k < channel->mNumPositionKeys; ++k) {
        animation::keyframe frame;
        frame.time = static_cast<float>(channel->mPositionKeys[k].mTime);
        frame.position = _convert_vec3(channel->mPositionKeys[k].mValue);
        frame.scale = (channel->mNumScalingKeys > k) ? _convert_vec3(channel->mScalingKeys[k].mValue) : math::vector3{1.0f};

        if (channel->mNumRotationKeys > k) {
          const auto& r = channel->mRotationKeys[k].mValue;
          frame.rotation = math::quaternion{r.x, r.y, r.z, r.w};
        } else {
          frame.rotation = math::quaternion::identity;
        }

        track.keyframes.push_back(frame);
      }

      clip.tracks.push_back(std::move(track));
    }

    return clip;
  }

private:

}; // class animation

} // namespace sbx::animations

#endif // LIBSBX_ANIMATIONS_ANIMATION_HPP_