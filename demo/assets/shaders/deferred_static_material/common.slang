#ifndef COMMON_SLANG_
#define COMMON_SLANG_

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
}; // struct material_data

struct static_instance_data {
  uint transform_index;
  uint material_index;
  uint object_id; 
  uint _pad0;
}; // struct instance_data

struct skinned_instance_data {
  uint transform_index;
  uint material_index;
  uint bone_matrix_offset;
  uint object_id;
}; // struct instance_data

interface vertex {
  float3 position();
  float3 normal();
  float2 uv();
}; // interface vertex

interface skinning {
  uint4 bone_indices();
  float4 bone_weights();
}; // interface skinning

struct static_vertex : vertex {
  float position_x;
  float position_y;
  float position_z;

  float normal_x;
  float normal_y;
  float normal_z;

  float uv_x;
  float uv_y;

  float tangent_x;
  float tangent_y;
  float tangent_z;
  float tangent_w; // sign of the tangent

  float3 position() {
    return float3(position_x, position_y, position_z);
  }

  float3 normal() {
    return float3(normal_x, normal_y, normal_z);
  }

  float2 uv() {
    return float2(uv_x, uv_x);
  }

}; // struct static_vertex

struct skinned_vertex : vertex, skinning {
  float position_x;
	float position_y;
	float position_z;

	float normal_x;
	float normal_y;
	float normal_z;

	float uv_x;
	float uv_y;

  float tangent_x;
  float tangent_y;
  float tangent_z;
  float tangent_w; // sign of the tangent

  uint bone_index_x;
  uint bone_index_y;
  uint bone_index_z;
  uint bone_index_w;

  float bone_weight_x;
  float bone_weight_y;
  float bone_weight_z;
  float bone_weight_w;

  float3 position() {
    return float3(position_x, position_y, position_z);
  }

  float3 normal() {
    return float3(normal_x, normal_y, normal_z);
  }

  float2 uv() {
    return float2(uv_x, uv_x);
  }

  uint4 bone_indices() {
    return uint4(bone_index_x, bone_index_y, bone_index_z, bone_index_w);
  }

  float4 bone_weights() {
    return float4(bone_weight_x, bone_weight_y, bone_weight_z, bone_weight_w);
  }

}; // struct skinned_vertex

struct static_push_data {
  static_vertex* vertex_buffer;
  transform_data* transform_data_buffer;
  material_data* material_data_buffer;
  static_instance_data* instance_data_buffer;
}; // struct push_data

struct skinned_push_data {
  skinned_vertex* vertex_buffer;
  transform_data* transform_data_buffer;
  material_data* material_data_buffer;
  skinned_instance_data* instance_data_buffer;
  float4x4* bone_matrices_buffer;
  uint bone_to_track;
}; // struct push_data

#endif // COMMON_SLANG_