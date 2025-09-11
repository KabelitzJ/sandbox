#include <libsbx/common/vertex.slang>

#define SKINNING 1

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

struct vertex : sbx::common::position<3>, sbx::common::normal<3>, sbx::common::tangent<4>, sbx::common::uv<2>, sbx::common::bone_indices<4>, sbx::common::bone_weights<4> {
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
  float tangent_w; // w: sign of the tangent
  uint bone_index_x;
  uint bone_index_y;
  uint bone_index_z;
  uint bone_index_w;
  float bone_weight_x;
  float bone_weight_y;
  float bone_weight_z;
  float bone_weight_w;

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

  vector<uint, 4> get_bone_indices() {
    return vector<uint, 4>(bone_index_x, bone_index_y, bone_index_z, bone_index_w);
  }

  vector<float, 4> get_bone_weights() {
    return vector<float, 4>(bone_weight_x, bone_weight_y, bone_weight_z, bone_weight_w);
  }

}; // struct vertex

struct push_data {
  vertex* vertex_buffer;
  transform_data* transform_data_buffer;
  instance_data* instance_data_buffer;
  float4x4* bone_matrices_buffer;
  uint bone_to_track;
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
  float3 tbn0 : TEXCOORD2;
  float3 tbn1 : TEXCOORD3;
  float3 tbn2 : TEXCOORD4;
  float2 uv : TEXCOORD5;
  float4 color : TEXCOORD6;
  float3 material : TEXCOORD7;
  nointerpolation uint2 image_indices : TEXCOORD8;
  nointerpolation uint2 object_id : TEXCOORD9;
  float4 sv_position : SV_Position;
}; // struct vs_out

float4x4 calculate_skinning_matrix(uint4 bone_indices, float4 bone_weights, uint bone_matrices_offset) {
#if (SKINNING == 1)
  // mat4 result = bone_weights[0] * bone_matrices_buffer.data[bone_indices[0] + bone_matrices_offset];
  // result += bone_weights[1] * bone_matrices_buffer.data[bone_indices[1] + bone_matrices_offset];
  // result += bone_weights[2] * bone_matrices_buffer.data[bone_indices[2] + bone_matrices_offset];
  // result += bone_weights[3] * bone_matrices_buffer.data[bone_indices[3] + bone_matrices_offset];
  // return result;

  float4x4 result = float4x4(
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0
  );

  for (int i = 0; i < 4; ++i) {
    result += bone_weights[i] * push.bone_matrices_buffer[bone_indices[i] + bone_matrices_offset];
  }

  return result;
#else
  float4x4 result = float4x4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  );
#endif
}

[shader("vertex")]
vs_out main(vs_in in) {
  vs_out out;

  uint global_vertex = in.vertex_id + in.base_vertex;
  uint global_instance = in.instance_id + in.base_instance;

  instance_data instance_data = push.instance_data_buffer[global_instance];

  uint2 image_indices = instance_data.payload.xy;
  uint transform_data_index = uint(instance_data.payload.z);
  uint bone_matrices_offset = uint(instance_data.payload.w);

  transform_data transform_data = push.transform_data_buffer[transform_data_index];

  vertex vertex = push.vertex_buffer[global_vertex];

  float3 in_position = sbx::common::get_position(vertex);
  float3 in_normal = sbx::common::get_normal(vertex);
  float4 in_tangent = sbx::common::get_tangent(vertex);
  float2 in_uv = sbx::common::get_uv(vertex);
  uint4 in_bone_indices = sbx::common::get_bone_indices(vertex);
  float4 in_bone_weights = sbx::common::get_bone_weights(vertex);

  float4x4 skinning_matrix = calculate_skinning_matrix(in_bone_indices, in_bone_weights, bone_matrices_offset);

  float3 skinned_position = mul(skinning_matrix, float4(in_position, 1.0)).xyz;
  float3 skinned_normal = normalize(mul(skinning_matrix, float4(in_normal, 0.0)).xyz);
  float3 skinned_tangent = normalize(mul(skinning_matrix, float4(in_tangent.xyz, 0.0)).xyz);

  float3 world_position = mul(transform_data.model, float4(skinned_position, 1.0)).xyz;

  float3 N = normalize(mul(transform_data.normal, float4(skinned_normal, 0.0)).xyz);
  float3 T = normalize(mul(transform_data.normal, float4(skinned_tangent, 0.0)).xyz);
  float3 B = cross(T, N) * in_tangent.w;

  // for (int i = 0; i < 4; ++i) {
  //   if (in_bone_indices[i] == bone_to_track) {
  //     if (in_bone_weights[i] > 0.0 && in_bone_weights[i] <= 0.3) {
  //       color = vec4(0.0, 1.0, 0.0, 1.0);
  //     } else if (in_bone_weights[i] > 0.3 && in_bone_weights[i] <= 0.6) {
  //       color = vec4(1.0, 1.0, 0.0, 1.0);
  //     } else if (in_bone_weights[i] > 0.6 && in_bone_weights[i] <= 1.0) {
  //       color = vec4(1.0, 0.0, 0.0, 1.0);
  //     }

  //     break;
  //   }
  // }

  // out_color = color;

  out.position = world_position;
  out.normal = N;
  out.tbn0 = T;
  out.tbn1 = B;
  out.tbn2 = N;
  out.uv = in_uv;
  out.color = instance_data.tint;
  out.material = instance_data.material.rgb;
  out.image_indices = image_indices;
  out.object_id = instance_data.selection.xy;

  out.sv_position = mul(scene.projection, mul(scene.view, float4(world_position, 1.0)));

  return out;
}
