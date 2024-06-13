#version 450

#include "../common/wind.glsl"
#include "../common/constants.glsl"

struct per_mesh_data {
  mat4 model;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 light_space;
  float time;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  gl_Position = scene.light_space * vec4(world_position, 1.0);
}
