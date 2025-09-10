#include <libsbx/common/image.slang>

struct fs_in {
  float3 position : TEXCOORD0;       // in_position
  float3 normal   : TEXCOORD1;       // in_normal
  float3 tbn0     : TEXCOORD2;       // T
  float3 tbn1     : TEXCOORD3;       // B
  float3 tbn2     : TEXCOORD4;       // N
  float2 uv       : TEXCOORD5;       // in_uv
  float4 color    : TEXCOORD6;       // in_color
  float3 material : TEXCOORD7;       // in_material (unused in this pass)
  nointerpolation uint3 image_indices : TEXCOORD8; // in_image_indices
  nointerpolation uint2 object_id     : TEXCOORD9; // in_object_id
  float4 sv_position : SV_Position;
}; // struct fs_in

struct fs_out {
  float4 accum     : SV_Target0; // out_accum
  float  revealage : SV_Target1; // out_revealage
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

  return color * in.color;
}

float3 get_normal(fs_in in) {
  uint index = in.image_indices.y;

  if (index >= sbx::common::max_image_array_size) {
    return normalize(in.normal);
  }

  float3 normal = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).xyz;

  return normalize(in.tbn0 * normal.x + in.tbn1 * normal.y + in.tbn2 * normal.z);
}

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  float4 albedo = get_albedo(in);

  // Equivalent to: clamp(pow(min(1, a*10)+0.01, 3)*1e8 * pow(1 - z*0.9, 3), 1e-2, 3e3)
  float a    = albedo.a;
  float w_a  = pow(min(1.0, a * 10.0) + 0.01, 3.0);
  float w_z  = pow(1.0 - in.sv_position.z * 0.9, 3.0);
  float weight = clamp(w_a * 1e8 * w_z, 1e-2, 3e3);

  out.accum     = float4(albedo.rgb * albedo.a, albedo.a) * weight;
  out.revealage = albedo.a;
  return out;
}
