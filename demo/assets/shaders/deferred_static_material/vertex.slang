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
  [[vk::location(7)]] nointerpolation uint2 object_id : OBJECT_ID;
  [[vk::location(8)]] float alpha_cutoff : ALPHA_CUTOFF;
  float4 sv_position : SV_Position;
}; // struct vs_out

[[vk::push_constant]] 
ConstantBuffer<push_data> push;

[[vk::binding(0, 0)]]
ConstantBuffer<sbx::common::scene_data> scene;

[shader("vertex")]
vs_out main(vs_in in) {
  vs_out out;

  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  vertex vertex = push.vertex_buffer[global_vertex];

  instance_data instance_data = push.instance_data_buffer[global_instance];

  transform_data transform_data = push.transform_data_buffer[instance_data.transform_index];
  material_data material_data = push.material_data_buffer[instance_data.material_index];

  float3 in_position = float3(vertex.position_x, vertex.position_y, vertex.position_z);
  float3 in_normal = float3(vertex.normal_x, vertex.normal_y, vertex.normal_z);
  float4 in_tangent = float4(vertex.tangent_x, vertex.tangent_y, vertex.tangent_z, vertex.tangent_w);
  float2 in_uv = float2(vertex.uv_x, vertex.uv_y);

  float3 world_position = mul(transform_data.model, float4(in_position, 1.0)).xyz;

  out.position = world_position;
  out.normal = normalize(mul(transform_data.normal, float4(in_normal, 0.0)).xyz);
  out.tangent = float4(normalize(mul(transform_data.normal, float4(in_tangent.xyz, 0.0)).xyz), in_tangent.w);
  out.uv = in_uv;
  out.color = material_data.base_color;
  out.material = float4(material_data.metallic, material_data.roughness, material_data.occlusion, material_data.emissive_strength);
  out.image_indices = uint4(material_data.albedo_index, material_data.normal_index, material_data.mrao_index, material_data.emissive_index);
  out.object_id = uint2(instance_data.object_id_upper, instance_data.object_id_lower);
  out.alpha_cutoff = material_data.alpha_cutoff;

  out.sv_position = mul(scene.projection, mul(scene.view, float4(world_position, 1.0)));

  return out;
}
