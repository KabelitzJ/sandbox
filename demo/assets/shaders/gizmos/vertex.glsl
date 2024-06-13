#version 450

#include "../common/wind.glsl"
#include "../common/constants.glsl"

struct per_mesh_data {
  mat4 model;
  vec4 tint;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 3) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  out_color = data.tint;

  gl_Position = scene.projection * scene.view * vec4(world_position, 1.0);
}
