#include <libsbx/common/vertex.slang>
#include <libsbx/common/convert_to.slang>
#include <libsbx/common/scene_data.slang>

#include <common.slang>

struct vs_in {
  uint vertex_id : SV_VertexID;
  uint instance_id : SV_InstanceID;
  uint base_vertex : SV_StartVertexLocation;
  uint base_instance : SV_StartInstanceLocation;
}; // struct vs_in

struct vs_out {
  [[vk::location(0)]] float3 position : POSITION;
  [[vk::location(1)]] float3 normal : NORMAL;
  [[vk::location(2)]] float4 tangent : TANGENT;
  [[vk::location(3)]] float2 uv : UV;
  [[vk::location(4)]] float4 color : COLOR;
  [[vk::location(5)]] float4 material : MATERIAL;
  [[vk::location(6)]] nointerpolation uint4 image_indices : IMAGE_INDICES;
  [[vk::location(7)]] nointerpolation uint object_id : OBJECT_ID;
  [[vk::location(8)]] float alpha_cutoff : ALPHA_CUTOFF;
  float4 sv_position : SV_Position;
}; // struct vs_out

[[vk::push_constant]] 
ConstantBuffer<static_push_data> static_push;

[[vk::push_constant]] 
ConstantBuffer<skinned_push_data> skinned_push;

[[vk::binding(0, 0)]]
ConstantBuffer<sbx::common::scene_data> scene;

float4x4 calculate_skinning_matrix(uint4 bone_indices, float4 bone_weights, float4x4* bone_matrices_buffer, uint bone_matrices_offset) {
  float4x4 result = float4x4(
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0
  );

  for (int i = 0; i < 4; ++i) {
    result += bone_weights[i] * bone_matrices_buffer[bone_indices[i] + bone_matrices_offset];
  }

  return result;
}

vs_out write(transform_data transform_data, material_data material_data, float3 position, float3 normal, float2 uv, float4 tangent, uint object_id) {
  vs_out out;

  out.position = position;
  out.normal = normalize(mul(transform_data.normal, float4(normal, 0.0)).xyz);
  out.tangent = float4(normalize(mul(transform_data.normal, float4(tangent.xyz, 0.0)).xyz), tangent.w);
  out.uv = uv;
  out.color = material_data.base_color;
  out.material = float4(material_data.metallic, material_data.roughness, material_data.occlusion, material_data.emissive_strength);
  out.image_indices = uint4(material_data.albedo_index, material_data.normal_index, material_data.mrao_index, material_data.emissive_index);
  out.object_id = object_id;
  out.alpha_cutoff = material_data.alpha_cutoff;

  out.sv_position = mul(scene.projection, mul(scene.view, float4(position, 1.0)));

  return out;
}

[shader("vertex")]
vs_out static_main(vs_in in) {
  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  static_vertex vertex = static_push.vertex_buffer[global_vertex];

  static_instance_data instance_data = static_push.instance_data_buffer[global_instance];

  transform_data transform_data = static_push.transform_data_buffer[instance_data.transform_index];
  material_data material_data = static_push.material_data_buffer[instance_data.material_index];

  float3 in_position = float3(vertex.position_x, vertex.position_y, vertex.position_z);
  float3 in_normal = float3(vertex.normal_x, vertex.normal_y, vertex.normal_z);
  float2 in_uv = float2(vertex.uv_x, vertex.uv_y);
  float4 in_tangent = float4(vertex.tangent_x, vertex.tangent_y, vertex.tangent_z, vertex.tangent_w);

  float3 world_position = mul(transform_data.model, float4(in_position, 1.0)).xyz;

  return write(transform_data, material_data, world_position, in_normal, in_uv, in_tangent, instance_data.object_id);
}

[shader("vertex")]
vs_out skinned_main(vs_in in) {
  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  skinned_vertex vertex = skinned_push.vertex_buffer[global_vertex];

  skinned_instance_data instance_data = skinned_push.instance_data_buffer[global_instance];

  transform_data transform_data = skinned_push.transform_data_buffer[instance_data.transform_index];
  material_data material_data = skinned_push.material_data_buffer[instance_data.material_index];

  uint bone_matrices_offset = uint(instance_data.bone_matrix_offset);

  float3 in_position = float3(vertex.position_x, vertex.position_y, vertex.position_z);
  float3 in_normal = float3(vertex.normal_x, vertex.normal_y, vertex.normal_z);
  float4 in_tangent = float4(vertex.tangent_x, vertex.tangent_y, vertex.tangent_z, vertex.tangent_w);
  float2 in_uv = float2(vertex.uv_x, vertex.uv_y);
  uint4 in_bone_indices = uint4(vertex.bone_index_x, vertex.bone_index_y, vertex.bone_index_z, vertex.bone_index_w);
  float4 in_bone_weights = float4(vertex.bone_weight_x, vertex.bone_weight_y, vertex.bone_weight_z, vertex.bone_weight_w);

  float4x4 skinning_matrix = calculate_skinning_matrix(in_bone_indices, in_bone_weights, skinned_push.bone_matrices_buffer, bone_matrices_offset);

  float3 skinned_position = mul(skinning_matrix, float4(in_position, 1.0)).xyz;
  float3 skinned_normal = normalize(mul(skinning_matrix, float4(in_normal, 0.0)).xyz);
  float3 skinned_tangent = normalize(mul(skinning_matrix, float4(in_tangent.xyz, 0.0)).xyz);

  float3 world_position = mul(transform_data.model, float4(skinned_position, 1.0)).xyz;
  float3 normal = normalize(mul(transform_data.normal, float4(skinned_normal, 0.0)).xyz);
  float4 tangent = float4(normalize(mul(transform_data.normal, float4(skinned_tangent, 0.0)).xyz), in_tangent.w);

  return write(transform_data, material_data, world_position, normal, in_uv, tangent, instance_data.object_id);
}