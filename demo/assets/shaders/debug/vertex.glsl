#version 450 core

struct per_mesh_data {
  mat4 model;
  vec4 tint;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(binding = 1, std430) readonly buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  out_position = vec3(data.model * vec4(in_position, 1.0));
  out_color = data.tint;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
