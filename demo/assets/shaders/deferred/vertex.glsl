#version 450 core 

#extension GL_EXT_buffer_reference : enable

#include <libsbx/common/wind.glsl>

struct per_mesh_data {
  mat4 model;
  mat4 normal;
  vec4 tint;
  vec4 material; // x: metallic, y: roughness, z: flexiblity, w: anchor height
  vec4 image_indices;
}; // struct per_mesh_data

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
  float tangent_w; // w: sign of the tangent
	float uv_x;
	float uv_y;
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

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out mat3 out_tbn; // Needs 3 locations slots (2, 3, 4)
layout(location = 5) out vec2 out_uv;
layout(location = 6) out vec4 out_color;
layout(location = 7) out vec2 out_material;
layout(location = 8) out flat uint out_albedo_image_index;
layout(location = 9) out flat uint out_normal_image_index;

layout(buffer_reference, std430) readonly buffer vertex_buffer { 
	vertex vertices[];
}; // buffer vertex_buffer

layout(push_constant) uniform constants {	
	vertex_buffer vertex_buffer;
} push_constants;

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

layout(set = 1, binding = 0, std430) readonly buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

const float MAX_ANCHOR_HEIGHT = 2.0;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vertex vertex = push_constants.vertex_buffer.vertices[gl_VertexIndex];

  vec3 in_position = position_from_vertex(vertex);
  vec3 in_normal = normal_from_vertex(vertex);
  vec4 in_tangent = tangent_from_vertex(vertex);
  vec2 in_uv = uv_from_vertex(vertex);

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  float flexibility = data.material.z;
  float anchor_height = data.material.w;

  // if (flexibility > 0.0) {
  //   out_position = wind_effect(world_position, in_position, scene.time, flexibility, anchor_height, MAX_ANCHOR_HEIGHT);
  // } else {
  //   out_position = world_position;
  // }

  out_position = world_position;

  out_normal = normalize(vec3(data.normal * vec4(in_normal, 1.0)));

  vec3 T = normalize(vec3(data.model * in_tangent));
  vec3 N = normalize(vec3(data.model * vec4(in_normal, 0.0)));
  vec3 B = cross(N, T) * in_tangent.w; // w: sign of the tangent

  out_tbn = mat3(T, B, N);

  out_uv = in_uv;

  out_color = data.tint;
  out_material = data.material.xy;

  out_albedo_image_index = uint(data.image_indices.x);
  out_normal_image_index = uint(data.image_indices.y);

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
