#version 460 core 

#extension GL_EXT_buffer_reference : enable
#extension GL_EXT_debug_printf : enable

#define MAX_BONES 64;

struct transform_data {
  mat4 model;
  mat4 normal;
}; // struct transform_data

struct instance_data {
  vec4 tint;
  vec4 material; // x: metallic, y: roughness, z: flexiblity, w: anchor height
  uvec4 payload; // x: albedo image index, y: normal image index, z: transform data index, w: bone matrices offset
  uvec4 selection; // x: upper 32 bit of id, y: lower 32 bit of id, z: unused, w: unused
}; // struct instance_data

struct vertex {
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
}; // struct vertex

vec3 position_from_vertex(vertex vertex) {
  return vec3(vertex.position_x, vertex.position_y, vertex.position_z);
}

vec3 normal_from_vertex(vertex vertex) {
  return vec3(vertex.normal_x, vertex.normal_y, vertex.normal_z);
}

vec4 tangent_from_vertex(vertex vertex) {
  return vec4(vertex.tangent_x, vertex.tangent_y, vertex.tangent_z, vertex.tangent_w);
}

vec2 uv_from_vertex(vertex vertex) {
  return vec2(vertex.uv_x, vertex.uv_y);
}

uvec4 bone_indices_from_vertex(vertex vertex) {
  return uvec4(vertex.bone_index_x, vertex.bone_index_y, vertex.bone_index_z, vertex.bone_index_w);
}

vec4 bone_weights_from_vertex(vertex vertex) {
  return vec4(vertex.bone_weight_x, vertex.bone_weight_y, vertex.bone_weight_z, vertex.bone_weight_w);
}

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out mat3 out_tbn; // Needs 3 locations slots (2, 3, 4)
layout(location = 5) out vec2 out_uv;
layout(location = 6) out vec4 out_color;
layout(location = 7) out vec2 out_material;
layout(location = 8) out flat uvec2 out_image_indices;
layout(location = 9) out flat uvec2 out_object_id;

layout(buffer_reference, std430) readonly buffer vertex_buffer_reference { 
	vertex data[];
}; // buffer vertex_buffer_reference

layout(buffer_reference, std430) readonly buffer transform_data_buffer_reference {
  transform_data data[];
}; // buffer transform_data_buffer_reference

layout(buffer_reference, std430) readonly buffer instance_data_buffer_reference {
  instance_data data[];
}; // buffer instance_data_buffer_reference

layout(buffer_reference, std430) readonly buffer bone_matrices_buffer_reference {
  mat4 data[];
}; // buffer bone_matrices_buffer_reference

layout(push_constant) uniform constants {	
	vertex_buffer_reference vertex_buffer;
  transform_data_buffer_reference transform_data_buffer;
  instance_data_buffer_reference instance_data_buffer;
  bone_matrices_buffer_reference bone_matrices_buffer;
  uint bone_to_track;
};

layout(set = 0, binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  // uint point_light_count;
  float time;
} scene;

// 0: no skinning
// 1: skinning enabled
#define SKINNING 1

mat4 calculate_skinning_matrix(uvec4 bone_indices, vec4 bone_weights, uint bone_matrices_offset) {
#if (SKINNING == 1)
  // mat4 result = bone_weights[0] * bone_matrices_buffer.data[bone_indices[0] + bone_matrices_offset];
  // result += bone_weights[1] * bone_matrices_buffer.data[bone_indices[1] + bone_matrices_offset];
  // result += bone_weights[2] * bone_matrices_buffer.data[bone_indices[2] + bone_matrices_offset];
  // result += bone_weights[3] * bone_matrices_buffer.data[bone_indices[3] + bone_matrices_offset];
  // return result;

  mat4 result = mat4(0.0);

  for (int i = 0; i < 4; ++i) {
    result += bone_weights[i] * bone_matrices_buffer.data[bone_indices[i] + bone_matrices_offset];
  }

  return result;
#else
  return mat4(1.0);
#endif
}

void main() {
  instance_data instance_data = instance_data_buffer.data[gl_InstanceIndex];

  uvec2 image_indices = instance_data.payload.xy;
  uint transform_data_index = uint(instance_data.payload.z);
  uint bone_matrices_offset = uint(instance_data.payload.w);

  transform_data transform_data = transform_data_buffer.data[transform_data_index];

  vertex vertex = vertex_buffer.data[gl_VertexIndex];

  vec3 in_position = position_from_vertex(vertex);
  vec3 in_normal = normal_from_vertex(vertex);
  vec4 in_tangent = tangent_from_vertex(vertex);
  vec2 in_uv = uv_from_vertex(vertex);
  uvec4 in_bone_indices = bone_indices_from_vertex(vertex);
  vec4 in_bone_weights = bone_weights_from_vertex(vertex);

  // in_bone_weights /= (in_bone_weights.x + in_bone_weights.y + in_bone_weights.z + in_bone_weights.w);

  mat4 skinning_matrix = calculate_skinning_matrix(in_bone_indices, in_bone_weights, bone_matrices_offset);

  vec3 skinned_position = vec3(skinning_matrix * vec4(in_position, 1.0));
  vec3 skinned_normal = normalize(vec3(skinning_matrix * vec4(in_normal, 0.0)));

  vec3 world_position = vec3(transform_data.model * vec4(skinned_position, 1.0));

  out_position = world_position;

  out_normal = normalize(vec3(transform_data.normal * vec4(skinned_normal, 0.0)));

  vec3 T = normalize(vec3(transform_data.model * vec4(in_tangent.xyz, 0.0)));
  vec3 N = normalize(vec3(transform_data.model * vec4(skinned_normal, 0.0)));
  vec3 B = cross(N, T) * in_tangent.w; // w: sign of the tangent

  out_tbn = mat3(T, B, N);

  out_uv = in_uv;

  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

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
  out_color = instance_data.tint;
  out_material = instance_data.material.xy;

  out_image_indices = image_indices;
  out_object_id = instance_data.selection.xy;

  gl_Position = scene.projection * scene.view * vec4(world_position, 1.0);
}
