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

interface fs_out {
  [mutating] bool write<AlphaPolicy: alpha_policy>(fs_in in);
}; // interface fs_out

struct opaque_fs_out : fs_out {

  float4 albedo : SV_Target0;
  float4 position : SV_Target1;
  float4 normal : SV_Target2;
  float4 material : SV_Target3;
  uint2 object_id : SV_Target4;
  float depth : SV_Target5;

  [mutating] bool write<AlphaPolicy: alpha_policy>(fs_in in) {
    this.albedo = get_albedo<AlphaPolicy>(in);

    if (AlphaPolicy::should_discard(this.albedo.a, in.alpha_cutoff)) {
      return false;
    }

    this.position = float4(in.position, 1.0);
    this.normal = float4(get_normal(in), 0.0);
    this.material = get_material(in);
    this.object_id = in.object_id;
    this.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

    return true;
  }

}; // struct opaque_fs_out

struct transparent_fs_out : fs_out {

  float4 accum : SV_Target0;
  float revealage : SV_Target1;

  [mutating] bool write<AlphaPolicy: alpha_policy>(fs_in in) {
    float4 albedo = get_albedo<AlphaPolicy>(in);

    // Equivalent to: clamp(pow(min(1, a*10)+0.01, 3)*1e8 * pow(1 - z*0.9, 3), 1e-2, 3e3)
    float a = albedo.a;
    float w_a = pow(min(1.0, a * 10.0) + 0.01, 3.0);
    float w_z = pow(1.0 - in.sv_position.z * 0.9, 3.0);
    float weight = clamp(w_a * 1e8 * w_z, 1e-2, 3e3);

    this.accum = float4(albedo.rgb * albedo.a, albedo.a) * weight;
    this.revealage = albedo.a;

    return true;
  }

}; // struct transparent_fs_out

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

[shader("fragment")]
FsOut main<AlphaPolicy: alpha_policy, FsOut: fs_out>(fs_in in) {
  FsOut out;

  if (!out.write<AlphaPolicy>(in)) {
    discard;
  }

  return out;
}
