#include <libsbx/common/depth.slang>
#include <libsbx/common/image.slang>
#include <libsbx/common/scene_data.slang>
#include <libsbx/common/texture_sampler.slang>

#include <common.slang>
#include <alpha_policy.slang>

struct fs_in {
  [[vk::location(0)]] float3 position : POSITION;
  [[vk::location(1)]] float3 normal : NORMAL;
  [[vk::location(2)]] float4 tangent : TANGENT;
  [[vk::location(3)]] float2 uv : UV;
  [[vk::location(4)]] nointerpolation uint instance_index : INSTANCE_INDEX;
  float4 sv_position : SV_Position;
}; // struct fs_in

struct opaque_fs_out {
  float4 albedo : SV_Target0;
  float4 position : SV_Target1;
  float4 normal : SV_Target2;
  float4 material : SV_Target3;
  float4 emissive : SV_Target4;
  uint object_id: SV_Target5;
  float depth : SV_Target6;
}; // struct opaque_fs_out

struct blend_fs_out {
  float4 accum : SV_Target0;
  float revealage : SV_Target1;
}; // struct blend_fs_out

[[vk::push_constant]]
ConstantBuffer<static_push_data> static_push;

[[vk::push_constant]]
ConstantBuffer<skinned_push_data> skinned_push;

[[vk::binding(0, 0)]]
ConstantBuffer<sbx::common::scene_data> scene;

[[vk::binding(1, 0)]]
SamplerState images_sampler;

[[vk::binding(2, 0)]]
Texture2D<float4> images[sbx::common::max_image_array_size];

float3 world_to_tangent(float3 v, float3 T, float3 B, float3 N) {
  return float3(dot(T, v), dot(B, v), dot(N, v));
}

float3 tangent_to_world(float3 v, float3 T, float3 B, float3 N) {
  return v.x * T + v.y * B + v.z * N;
}

void build_tbn(float3 position, float3 normal, float4 tangent, out float3 T, out float3 B, out float3 N)
{
  N = normalize(normal);
  T = normalize(tangent.xyz);

  T = normalize(T - N * dot(T, N));

  B = normalize(cross(N, T)) * tangent.w;
}

float2 parallax_mapping(float3 T, float3 B, float3 N, float2 uv, float3 position_ws, material_data material_data) {
  uint index = material_data.height_index;

  if (index >= sbx::common::max_image_array_size) {
    return uv;
  }

  float3 view_dir_ws = normalize(scene.camera_position - position_ws);

  float3 view_direction_ts = world_to_tangent(view_dir_ws, T, B, N);

  view_direction_ts.z = max(view_direction_ts.z, 0.05);
  view_direction_ts = normalize(view_direction_ts);

  // Parallax occlusion parameters
  float ndotv_ts = max(dot(float3(0.0, 0.0, 1.0), view_direction_ts), 0.0);
  float layer_count = lerp(material_data.parallax_min_layers, material_data.parallax_max_layers, ndotv_ts);

  float layer_depth = 1.0 / layer_count;
  float current_layer_depth = 0.0;

  float2 P = view_direction_ts.xy * material_data.height_scale;
  float2 uv_delta = P / layer_count;

  float2 current_uv = uv;
  float current_depth_map_value = images[NonUniformResourceIndex(index)].Sample(images_sampler, current_uv).r;

  [loop]
  while (current_layer_depth < current_depth_map_value) {
    current_uv -= uv_delta;
    current_layer_depth += layer_depth;
    current_depth_map_value = images[NonUniformResourceIndex(index)].Sample(images_sampler, current_uv).r;
  }

  float2 previous_uv = current_uv + uv_delta;

  float after_depth  = current_depth_map_value - current_layer_depth;
  float before_depth = images[NonUniformResourceIndex(index)].Sample(images_sampler, previous_uv).r - (current_layer_depth - layer_depth);

  float weight = after_depth / (after_depth - before_depth);

  float2 final_uv = previous_uv * weight + current_uv * (1.0 - weight);

  return final_uv;
}

float4 get_albedo<AlphaPolicy: alpha_policy>(float2 uv, material_data material_data) {
  uint index = material_data.albedo_index;

  if (index >= sbx::common::max_image_array_size) {
    return material_data.base_color;
  }

  float4 color = images[NonUniformResourceIndex(index)].Sample(images_sampler, uv);

  return AlphaPolicy::combine_colors(color, material_data.base_color);
}

float3 get_normal(float3 T, float3 B, float3 N, float2 uv, float3 normal_ws, material_data material_data) {
  uint index = material_data.normal_index;

  if (index >= sbx::common::max_image_array_size) {
    return normalize(normal_ws);
  }

  float3 sampled_normal = images[NonUniformResourceIndex(index)].Sample(images_sampler, uv).xyz;
  sampled_normal = sampled_normal * 2.0 - 1.0;
  sampled_normal.xy *= material_data.normal_scale;
  sampled_normal = normalize(sampled_normal);

  return normalize(tangent_to_world(sampled_normal, T, B, N));
}

float3 get_material(float2 uv, material_data material_data) {
  uint index = material_data.mrao_index;

  if (index >= sbx::common::max_image_array_size) {
    return float3(material_data.metallic, material_data.roughness, material_data.occlusion);
  }

  return images[NonUniformResourceIndex(index)].Sample(images_sampler, uv).rgb;
}

float3 get_emissive(float2 uv, material_data material_data) {
  uint index = material_data.emissive_index;

  float3 emissive = material_data.emissive_factor.rgb * material_data.emissive_strength;

  if (index >= sbx::common::max_image_array_size) {
    return emissive;
  }

  return emissive * images[NonUniformResourceIndex(index)].Sample(images_sampler, uv).rgb;
}

opaque_fs_out opaque_main(fs_in in, material_data material_data, uint object_id) {
  opaque_fs_out out;

  float3 T, B, N;
  build_tbn(in.position, in.normal, in.tangent, T, B, N);

  float2 parallax_uv = parallax_mapping(T, B, N, in.uv, in.position, material_data);

  out.albedo = get_albedo<opaque_alpha_policy>(parallax_uv, material_data);
  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(T, B, N, parallax_uv, in.normal, material_data), 0.0);
  out.material = float4(get_material(parallax_uv, material_data), 0.0);
  out.emissive = float4(get_emissive(parallax_uv, material_data), 0.0);
  out.object_id = object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, scene.camera_near, scene.camera_far);

  return out;
}

[earlydepthstencil]
[shader("fragment")]
opaque_fs_out static_opaque_main(fs_in in) {
  uint global_instance = in.instance_index;

  static_instance_data instance_data = static_push.instance_data_buffer[global_instance];
  material_data material_data = static_push.material_data_buffer[instance_data.material_index];

  return opaque_main(in, material_data, instance_data.object_id);
}

[earlydepthstencil]
[shader("fragment")]
opaque_fs_out skinned_opaque_main(fs_in in) {
  uint global_instance = in.instance_index;

  skinned_instance_data instance_data = skinned_push.instance_data_buffer[global_instance];
  material_data material_data = skinned_push.material_data_buffer[instance_data.material_index];

  return opaque_main(in, material_data, instance_data.object_id);
}

opaque_fs_out mask_main(fs_in in, material_data material_data, uint object_id) {
  opaque_fs_out out;

  float3 T, B, N;
  build_tbn(in.position, in.normal, in.tangent, T, B, N);

  float2 parallax_uv = parallax_mapping(T, B, N, in.uv, in.position, material_data);

  float4 albedo = get_albedo<mask_alpha_policy>(parallax_uv, material_data);

  if (albedo.a < material_data.alpha_cutoff) {
    discard;
  }

  out.albedo = albedo;
  out.position = float4(in.position, 1.0);
  out.normal = float4(get_normal(T, B, N, parallax_uv, in.normal, material_data), 0.0);
  out.material = float4(get_material(parallax_uv, material_data), 0.0);
  out.emissive = float4(get_emissive(parallax_uv, material_data), 0.0);
  out.object_id = object_id;
  out.depth = sbx::common::linearize_depth(in.sv_position.z, scene.camera_near, scene.camera_far);

  return out;
}

[shader("fragment")]
opaque_fs_out static_mask_main(fs_in in) {
  uint global_instance = in.instance_index;

  static_instance_data instance_data = static_push.instance_data_buffer[global_instance];
  material_data material_data = static_push.material_data_buffer[instance_data.material_index];

  return mask_main(in, material_data, instance_data.object_id);
}

[shader("fragment")]
opaque_fs_out skinned_mask_main(fs_in in) {
  uint global_instance = in.instance_index;

  skinned_instance_data instance_data = skinned_push.instance_data_buffer[global_instance];
  material_data material_data = skinned_push.material_data_buffer[instance_data.material_index];

  return mask_main(in, material_data, instance_data.object_id);
}

blend_fs_out blend_main(fs_in in, material_data material_data, uint object_id) {
  blend_fs_out out;
  
  float4 albedo = get_albedo<blend_alpha_policy>(in.uv, material_data);

  // Equivalent to: clamp(pow(min(1, a*10)+0.01, 3)*1e8 * pow(1 - z*0.9, 3), 1e-2, 3e3)
  float alpha = albedo.a;

  float w_alpha = pow(min(1.0, alpha * 10.0) + 0.01, 3.0);
  float w_depth = pow(1.0 - in.sv_position.z * 0.9, 3.0);

  float weight = clamp(w_alpha * 1e8 * w_depth, 1e-2, 3e3);

  out.accum = float4(albedo.rgb * albedo.a, albedo.a) * weight;
  out.revealage = albedo.a;

  return out;
}

[shader("fragment")]
blend_fs_out static_blend_main(fs_in in) {
  uint global_instance = in.instance_index;

  static_instance_data instance_data = static_push.instance_data_buffer[global_instance];
  material_data material_data = static_push.material_data_buffer[instance_data.material_index];

  return blend_main(in, material_data, instance_data.object_id);
}

[shader("fragment")]
blend_fs_out skinned_blend_main(fs_in in) {
  uint global_instance = in.instance_index;

  skinned_instance_data instance_data = skinned_push.instance_data_buffer[global_instance];
  material_data material_data = skinned_push.material_data_buffer[instance_data.material_index];

  return blend_main(in, material_data, instance_data.object_id);
}
