#include <libsbx/common/constants.slang>

struct fs_in {
  float2 uv : TEXCOORD0;
  float4 sv_position : SV_Position;
}; // struct fs_in

struct fs_out {
  float4 color : SV_Target0;
}; // struct fs_out

struct scene_data {
  float3 camera_position;
  float3 light_direction;
  float4 light_color;
  float4x4 light_space;
  uint point_light_count;
}; // struct scene_data

[[vk::binding(0, 0)]]
ConstantBuffer<scene_data> scene;

struct point_light {
  float3 position;
  float radius;
  float4 color; // rgb + intensity in .a
}; // struct point_light

[[vk::binding(1, 0)]]
StructuredBuffer<point_light> buffer_point_lights;

[[vk::binding(2, 0)]] Sampler2D<float4> accum_image;
[[vk::binding(3, 0)]] Sampler2D<float4> revealage_image;

bool is_approximately_equal(float a, float b) {
  return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * sbx::common::epsilon;
}

float max3(float3 vector) {
  return max(max(vector.x, vector.y), vector.z);
}

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  float _keep = 0.0;
  _keep += scene.camera_position.x * 0.01;
  if (scene.point_light_count > 0) {
    _keep += buffer_point_lights[0].position.x * 0.02;
  }

  // fragment coordination
  int2 coords = int2(in.sv_position.xy);

  // fragment revealage
  float revealage = revealage_image.Load(int3(coords, 0)).r;

  // save the blending and color texture fetch cost if there is not a transparent fragment
  if (is_approximately_equal(revealage, 1.0f)) {
    discard;
  }

  // fragment color
  float4 accumulation = accum_image.Load(int3(coords, 0));

  // suppress overflow
  if (isinf(max3(abs(accumulation.rgb)))) {
    accumulation.rgb = float3(accumulation.a);
  }

  // prevent floating point precision bug
  float3 average = accumulation.rgb / max(accumulation.a, sbx::common::epsilon);

  // blend pixels
  out.color = float4(average, 1.0f - revealage) + _keep - _keep;

  return out;
}