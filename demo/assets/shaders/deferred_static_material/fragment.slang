#include <libsbx/common/depth.slang>
#include <libsbx/common/image.slang>
#include <libsbx/common/texture_sampler.slang>

// #ifndef SBX_FEATURE_NORMAL_MAP
// #define SBX_FEATURE_NORMAL_MAP 0
// #endif // SBX_FEATURE_NORMAL_MAP

// #ifndef SBX_FEATURE_ALPHA_CLIP
// #define SBX_FEATURE_ALPHA_CLIP 0
// #endif // SBX_FEATURE_ALPHA_CLIP

struct fs_in {
  float3 position : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 tbn0 : TEXCOORD2;
  float3 tbn1 : TEXCOORD3;
  float3 tbn2 : TEXCOORD4;
  float2 uv : TEXCOORD5;
  float4 color : TEXCOORD6;
  float4 material : TEXCOORD7;
  nointerpolation uint4 image_indices : TEXCOORD8;
  nointerpolation uint2 object_id : TEXCOORD9;
  float alpha_cutoff : TEXCOORD10;
  float4 sv_position : SV_Position;
}; // struct fs_in

struct fs_out {
  float4 albedo : SV_Target0;
  float4 position : SV_Target1;
  float4 normal : SV_Target2;
  float4 material : SV_Target3;
  uint2 object_id : SV_Target4;
  float depth : SV_Target5;
}; // struct fs_out

[[vk::binding(1, 0)]]
SamplerState images_sampler;

[[vk::binding(2, 0)]]
Texture2D<float4> images[sbx::common::max_image_array_size];

float4 get_albedo(fs_in in) {
  uint index = in.image_indices.x;

  if (index >= sbx::common::max_image_array_size) {
    return in.color;
  }

  float4 color = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv);

#if SBX_FEATURE_ALPHA_CLIP
  return float4(color.rgb * in.color.rgb, color.a);
#elif SBX_FEATURE_ALPHA
  return color * in.color;
#else
  return float4(color.rgb * in.color.rgb, 1.0);
#endif // SBX_FEATURE_ALPHA_CLIP
}

float3 get_normal(fs_in in) {
#if SBX_FEATURE_NORMAL_MAP
  uint index = in.image_indices.y;

  if (index >= sbx::common::max_image_array_size) {
    return normalize(in.normal);
  }

  float3 normal = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).xyz;

  return normalize(in.tbn0 * normal.x + in.tbn1 * normal.y + in.tbn2 * normal.z);
#else
  return normalize(in.normal);
#endif // SBX_FEATURE_NORMAL_MAP
}

float4 get_material(fs_in in) {
  uint index = in.image_indices.z;

  if (index >= sbx::common::max_image_array_size) {
    return in.material;
  }

  return images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).rgba;
}

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  out.albedo = get_albedo(in);

#if SBX_FEATURE_ALPHA_CLIP
  // if (out.albedo.a < in.alpha_cutoff) {
  //   // discard;
  //   out.albedo = float4(1, 0, 1, 1);
  // }

  out.albedo = float4(1, 0, 1, 1);
#endif // SBX_FEATURE_ALPHA_CLIP

  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(in), 0.0);
  out.material = get_material(in);
  out.object_id = in.object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

  return out;
}
