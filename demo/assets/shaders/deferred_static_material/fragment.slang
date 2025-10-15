#include <libsbx/common/depth.slang>
#include <libsbx/common/image.slang>
#include <libsbx/common/texture_sampler.slang>

#include <common.slang>
#include <alpha_policy.slang>

struct fs_in {
  [[vk::location(0)]] float3 position : POSITION;
  [[vk::location(1)]] float3 normal : NORMAL;
  [[vk::location(2)]] float3 tbn0 : TBN0;
  [[vk::location(3)]] float3 tbn1 : TBN1;
  [[vk::location(4)]] float3 tbn2 : TBN2;
  [[vk::location(5)]] float2 uv : UV;
  [[vk::location(6)]] float4 color : COLOR;
  [[vk::location(7)]] float4 material : MATERIAL;
  [[vk::location(8)]] nointerpolation uint4 image_indices : IMAGE_INDICES;
  [[vk::location(9)]] nointerpolation uint2 object_id : OBJECT_ID;
  [[vk::location(10)]] float alpha_cutoff : ALPHA_CUTOFF;
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

  return normalize(in.tbn0 * normal.x + in.tbn1 * normal.y + in.tbn2 * normal.z);
}

float4 get_material(fs_in in) {
  uint index = in.image_indices.z;

  if (index >= sbx::common::max_image_array_size) {
    return in.material;
  }

  return images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).rgba;
}

[shader("fragment")]
fs_out main<AlphaPolicy: alpha_policy>(fs_in in) {
  fs_out out;

  out.albedo = get_albedo<AlphaPolicy>(in);

  if (AlphaPolicy::should_discard(out.albedo.a, in.alpha_cutoff)) {
    discard;
  }

  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(in), 0.0);
  out.material = get_material(in);
  out.object_id = in.object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

  return out;
}
