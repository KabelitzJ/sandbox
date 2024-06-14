#version 450

#include "../common/wind.glsl"
#include "../common/constants.glsl"

struct per_mesh_data {
  mat4 model;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 light_space;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  gl_Position = scene.light_space * data.model * vec4(in_position, 1.0);
}
