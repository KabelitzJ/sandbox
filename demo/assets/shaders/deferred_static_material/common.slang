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

#endif // COMMON_SLANG_