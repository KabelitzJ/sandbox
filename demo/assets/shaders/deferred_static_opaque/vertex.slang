#include <libsbx/common/vertex.slang>
#include <libsbx/common/convert_to.slang>
#include <libsbx/common/scene_data.slang>

struct transform_data {
  float4x4 model;
  float4x4 normal;
}; // struct transform_data

struct material_factors : sbx::common::convert_to<float3> {
  float metallic;
  float roughness;
  float occlusion;
  float _unused0;

  float3 to() {
    return float3(metallic, roughness, occlusion);
  }

}; // struct material_factors

struct index_data : sbx::common::convert_to<uint3> {
  uint albedo_image;
  uint normal_image;
  uint mrao_image;
  uint transform_data;

  uint3 to() {
    return uint3(albedo_image, normal_image, mrao_image);
  }

}; // struct index_data


struct selection_data : sbx::common::convert_to<uint2> {
  uint upper;
  uint lower;
  uint _unused0;
  uint _unused1;

  uint2 to() {
    return uint2(upper, lower);
  }

}; // struct selection_data


struct instance_data {
  float4 tint;
  material_factors material_factors;
  index_data index_data;
  selection_data selection_data;
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
  float3 material : TEXCOORD7;
  nointerpolation uint3 image_indices : TEXCOORD8;
  nointerpolation uint2 object_id : TEXCOORD9;
  float4 sv_position : SV_Position;
}; // struct vs_out

[shader("vertex")]
vs_out main(vs_in in) {
  vs_out out;

  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  instance_data instance_data = push.instance_data_buffer[global_instance];
  transform_data transform_data = push.transform_data_buffer[instance_data.index_data.transform_data];
  vertex vertex = push.vertex_buffer[global_vertex];

  float3 in_position = sbx::common::get_position(vertex);
  float3 in_normal = sbx::common::get_normal(vertex);
  float4 in_tangent = sbx::common::get_tangent(vertex);
  float2 in_uv = sbx::common::get_uv(vertex);

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
  out.color = instance_data.tint;
  out.material = sbx::common::to<float3>(instance_data.material_factors);
  out.image_indices = sbx::common::to<uint3>(instance_data.index_data);
  out.object_id = sbx::common::to<uint2>(instance_data.selection_data);

  out.sv_position = mul(scene.projection, mul(scene.view, float4(world_position, 1.0)));

  return out;
}
