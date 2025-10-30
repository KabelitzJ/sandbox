#include <libsbx/common/depth.slang>
#include <libsbx/common/image.slang>
#include <libsbx/common/texture_sampler.slang>

#include <common.slang>
#include <alpha_policy.slang>

struct fs_in {
  [[vk::location(0)]] float3 position : POSITION;
  [[vk::location(1)]] float3 normal : NORMAL;
  [[vk::location(2)]] float4 tangent : TANGENT;
  [[vk::location(3)]] float2 uv : UV;
  [[vk::location(4)]] float4 color : COLOR;
  [[vk::location(5)]] float4 material : MATERIAL;
  [[vk::location(6)]] nointerpolation uint4 image_indices : IMAGE_INDICES;
  [[vk::location(7)]] nointerpolation uint2 object_id : OBJECT_ID;
  [[vk::location(8)]] float alpha_cutoff : ALPHA_CUTOFF;
  float4 sv_position : SV_Position;
}; // struct fs_in

struct opaque_fs_out {
  float4 albedo : SV_Target0;
  float4 position : SV_Target1;
  float4 normal : SV_Target2;
  float4 material : SV_Target3;
  uint2 object_id : SV_Target4;
  float depth : SV_Target5;
}; // struct opaque_fs_out

struct blend_fs_out {
  float4 accum : SV_Target0;
  float revealage : SV_Target1;
}; // struct blend_fs_out

[[vk::binding(1, 0)]]
SamplerState images_sampler;

[[vk::binding(2, 0)]]
Texture2D<float4> images[sbx::common::max_image_array_size];

float4 get_albedo<AlphaPolicy: alpha_policy>(fs_in in) {
  uint index = in.image_indices.x;

  if (index >= sbx::common::max_image_array_size) {
    return in.color;
  }

  float4 color = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv);

  return AlphaPolicy::combine_colors(color, in.color);
}

float3 get_normal(fs_in in) {
  uint index = in.image_indices.y;

  if (index >= sbx::common::max_image_array_size) {
    return normalize(in.normal);
  }

  float3 normal = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).xyz;

  float3 N = in.normal;
  float3 T = in.tangent.xyz;
  float3 B = cross(T, N) * in.tangent.w;

  return normalize(T * normal.x + B * normal.y + N * normal.z);
}

float4 get_material(fs_in in) {
  uint index = in.image_indices.z;

  if (index >= sbx::common::max_image_array_size) {
    return in.material;
  }

  return images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).rgba;
}

[earlydepthstencil]
[shader("fragment")]
opaque_fs_out opaque_main(fs_in in) {
  opaque_fs_out out;

  out.albedo = get_albedo<opaque_alpha_policy>(in);
  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(in), 0.0);
  out.material = get_material(in);
  out.object_id = in.object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

  return out;
}

[shader("fragment")]
opaque_fs_out mask_main(fs_in in) {
  opaque_fs_out out;

  float4 albedo = get_albedo<mask_alpha_policy>(in);

  if (albedo.a < in.alpha_cutoff) {
    discard;
  }

  out.albedo = albedo;
  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(in), 0.0);
  out.material = get_material(in);
  out.object_id = in.object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

  return out;
}

[shader("fragment")]
blend_fs_out blend_main(fs_in in) {
  blend_fs_out out;
  
  float4 albedo = get_albedo<blend_alpha_policy>(in);

  // Equivalent to: clamp(pow(min(1, a*10)+0.01, 3)*1e8 * pow(1 - z*0.9, 3), 1e-2, 3e3)
  float alpha = albedo.a;
  
  float w_alpha = pow(min(1.0, alpha * 10.0) + 0.01, 3.0);
  float w_depth = pow(1.0 - in.sv_position.z * 0.9, 3.0);

  float weight = clamp(w_alpha * 1e8 * w_depth, 1e-2, 3e3);

  out.accum = float4(albedo.rgb * albedo.a, albedo.a) * weight;
  out.revealage = albedo.a;

  return out;
}
