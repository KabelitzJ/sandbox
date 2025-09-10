#include <libsbx/common/depth.slang>
#include <libsbx/common/image.slang>

struct fs_in {
  float3 position : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 tbn0 : TEXCOORD2;
  float3 tbn1 : TEXCOORD3;
  float3 tbn2 : TEXCOORD4;
  float2 uv : TEXCOORD5;
  float4 color : TEXCOORD6;
  float3 material : TEXCOORD7;
  nointerpolation uint3 image_indices : TEXCOORD8;
  nointerpolation uint2 object_id : TEXCOORD9;
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
Texture2D images[sbx::common::max_image_array_size];

float4 get_albedo(fs_in in) {
  uint index = in.image_indices.x;

  if (index >= sbx::common::max_image_array_size) {
    return in.color;
  }

  float4 color = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv);

  return float4(color.rgb * in.color.rgb, 1.0);
}

float3 get_normal(fs_in in) {
  uint index = in.image_indices.y;

  if (index >= sbx::common::max_image_array_size) {
    return normalize(in.normal);
  }

  float3 normal = images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).xyz;

  return normalize(in.tbn0 * normal.x + in.tbn1 * normal.y + in.tbn2 * normal.z);
}

float3 get_material(fs_in in) {
  uint index = in.image_indices.z;

  if (index >= sbx::common::max_image_array_size) {
    return in.material;
  }

  return images[NonUniformResourceIndex(index)].Sample(images_sampler, in.uv).rgb;
}

[shader("fragment")]
fs_out main(fs_in in) {
  fs_out out;

  out.albedo = get_albedo(in);
  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(in), 0.0);
  out.material = float4(get_material(in), 0.0);
  out.object_id = in.object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, sbx::common::default_near, sbx::common::default_far);

  return out;
}
