#include <libsbx/common/vertex.slang>

struct transform_data {
  float4x4 model;
  float4x4 normal;
}; // struct transform_data

struct instance_data {
  float4 tint;
  float4 material;  // x: metallic, y: roughness, z: ao, w: unused
  uint4 payload;    // x: albedo idx, y: normal idx, z: mrao idx, w: transform idx
  uint4 selection;  // x: upper32 id, y: lower32 id
}; // struct instance_data

struct vertex : sbx::common::position<3>, sbx::common::normal<3>, sbx::common::tangent<4>, sbx::common::uv<2> {
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

  vector<float, 3> get_position() {
    return vector<float, 3>(position_x, position_y, position_z);
  }

  vector<float, 3> get_normal() {
    return vector<float, 3>(normal_x, normal_y, normal_z);
  }

  vector<float, 4> get_tangent() {
    return vector<float, 4>(tangent_x, tangent_y, tangent_z, tangent_w);
  }

  vector<float, 2> get_uv() {
    return vector<float, 2>(uv_x, uv_y);
  }

}; // struct vertex

struct push_data {
  vertex* vertex_buffer;
  transform_data* transform_data_buffer;
  instance_data* instance_data_buffer;
}; // struct push_data

[[vk::push_constant]] 
ConstantBuffer<push_data> push;

struct scene_data {
  float4x4 view;
  float4x4 projection;
  float3 camera_position;
  float3 light_direction;
  float4 light_color;
  float4x4 light_space;
  float time;
}; // struct scene_data

[[vk::binding(0, 0)]]
ConstantBuffer<scene_data> scene;

struct vs_in {
  uint vertex_id : SV_VertexID;
  uint instance_id : SV_InstanceID;
  uint base_vertex : SV_StartVertexLocation;
  uint base_instance : SV_StartInstanceLocation;
}; // struct vs_in

struct vs_out {
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
}; // struct vs_out

[shader("vertex")]
vs_out main(vs_in in) {
  vs_out out;

  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  instance_data instance_data = push.instance_data_buffer[global_instance];
  transform_data transform_data = push.transform_data_buffer[instance_data.payload.w];
  vertex vertex = push.vertex_buffer[global_vertex];

  float3 in_position = sbx::common::get_position(vertex);
  float3 in_normal = sbx::common::get_normal(vertex);
  float4 in_tangent = sbx::common::get_tangent(vertex);
  float2 in_uv = sbx::common::get_uv(vertex);

  float3 world_position = mul(transform_data.model, float4(in_position, 1.0)).xyz;

  out.position = world_position;
  out.normal = normalize(mul(transform_data.normal, float4(in_normal, 0.0)).xyz);;
  out.tangent = normalize(mul(transform_data.normal, float4(in_tangent.xyz, 0.0)).xyz);
  out.bitangent = cross(out.tangent, out.normal) * in_tangent.w;
  out.uv = in_uv;
  out.color = instance_data.tint;
  out.material = instance_data.material.xyz;
  out.image_indices = uint3(instance_data.payload.x, instance_data.payload.y, instance_data.payload.z);
  out.object_id = uint2(instance_data.selection.x, instance_data.selection.y);

  out.sv_position = mul(scene.projection, mul(scene.view, float4(world_position, 1.0)));

  return out;
}
