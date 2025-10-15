#include <libsbx/common/vertex.slang>
#include <libsbx/common/convert_to.slang>
#include <libsbx/common/scene_data.slang>

struct transform_data {
  float4x4 model;
  float4x4 normal;
}; // struct transform_data

struct material_data {
  uint albedo_index;
  uint normal_index;
  uint mrao_index;
  uint emissive_index;

  float4 base_color;
  float4 emissive_color;

  float metallic;
  float roughness;
  float occlusion;
  float emissive_strength;

  float alpha_cutoff;
  float normal_scale;
  uint flags;
  uint _pad0;
}; // struct material_header

struct instance_data {
  uint transform_index;
  uint material_index;
  uint object_id_upper;
  uint object_id_lower;
}; // struct instance_data

struct vertex {
  float position_x;
  float position_y;
  float position_z;
  float normal_x;

  float normal_y;
  float normal_z;
  float tangent_x;
  float tangent_y;

  float tangent_z;
  float tangent_w;   // sign
  float uv_x;
  float uv_y;
}; // struct vertex

struct push_data {
  vertex* vertex_buffer;
  transform_data* transform_data_buffer;
  material_data* material_data_buffer;
  instance_data* instance_data_buffer;
}; // struct push_data

[[vk::push_constant]] 
ConstantBuffer<push_data> push;

[[vk::binding(0, 0)]]
ConstantBuffer<sbx::common::scene_data> scene;

struct vs_in {
  uint vertex_id : SV_VertexID;
  uint instance_id : SV_InstanceID;
  uint base_vertex : SV_StartVertexLocation;
  uint base_instance : SV_StartInstanceLocation;
}; // struct vs_in

struct vs_out {
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
}; // struct vs_out

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

  float3 N = normalize(mul(transform_data.normal, float4(in_normal, 0.0)).xyz);
  float3 T = normalize(mul(transform_data.normal, float4(in_tangent.xyz, 0.0)).xyz);
  float3 B = cross(T, N) * in_tangent.w;

  out.position = world_position;
  out.normal = N;
  out.tbn0 = T;
  out.tbn1 = B;
  out.tbn2 = N;
  out.uv = in_uv;
  out.color = material_data.base_color;
  out.material = float4(material_data.metallic, material_data.roughness, material_data.occlusion, material_data.emissive_strength);
  out.image_indices = uint4(material_data.albedo_index, material_data.normal_index, material_data.mrao_index, material_data.emissive_index);
  out.object_id = uint2(instance_data.object_id_upper, instance_data.object_id_lower);

  out.sv_position = mul(scene.projection, mul(scene.view, float4(world_position, 1.0)));

  return out;
}
