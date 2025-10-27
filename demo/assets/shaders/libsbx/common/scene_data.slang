#ifndef COMMON_SCENE_DATA_SLANG_
#define COMMON_SCENE_DATA_SLANG_

namespace sbx::common {

struct scene_data {
  float4x4 view;
  float4x4 projection;
  float3 camera_position;
  float3 light_direction;
  float4 light_color;
  float4x4 light_space;
  uint point_light_count;
  float time;
}; // struct scene_data

} // namespace sbx::common

#endif // COMMON_SCENE_DATA_SLANG_
