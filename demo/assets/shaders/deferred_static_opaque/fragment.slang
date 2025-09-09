#include <libsbx/common/depth.slang>

#define MAX_IMAGE_ARRAY_SIZE 64

struct fs_in {
  float3 position : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 tangent : TEXCOORD2;
  float3 bitangent : TEXCOORD3;
  float2 uv : TEXCOORD4;
  float4 color : TEXCOORD5;
  float3 material : TEXCOORD6;
  nointerpolation uint3 image_indices : TEXCOORD9;
  nointerpolation uint2 object_id : TEXCOORD8;
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
SamplerState images_sampler : register(s1, space0);
[[vk::binding(2, 0)]]
Texture2D images[MAX_IMAGE_ARRAY_SIZE] : register(t2, space0);

static const float DEFAULT_NEAR = 0.1f;
static const float DEFAULT_FAR  = 1000.0f;

float4 get_albedo(fs_in in) {
  uint index = in.image_indices.x;

  if (index >= MAX_IMAGE_ARRAY_SIZE) {
    return in.color;
  }

  float4 color = images[index].Sample(images_sampler, in.uv);

  return float4(color.rgb * in.color.rgb, 1.0);
}

float3 get_normal(fs_in in) {
  uint index = in.image_indices.y;

  if (index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in.normal);
  }

  float3 normal = images[index].Sample(images_sampler, in.uv).xyz;
  float3x3 tbn = float3x3(in.tangent, in.bitangent, in.normal);

  return normalize(mul(tbn, normal));
}

float3 get_material(fs_in in) {
  uint index = in.image_indices.z;

  if (index >= MAX_IMAGE_ARRAY_SIZE) {
    return in.material;
  }

  return images[index].Sample(images_sampler, in.uv).rgb;
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
