#version 460 core

#include <libsbx/common/wind.glsl>
#include <libsbx/common/constants.glsl>

struct per_mesh_data {
  mat4 model;
  vec4 material; // x: metallic, y: roughness, z: flexiblity, w: anchor height
  vec4 image_indices;
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 1) out vec2 out_uv;
layout(location = 2) out flat uint out_albedo_image_index;

layout(binding = 0) uniform uniform_scene {
  mat4 light_space;
  float time;
} scene;

layout(binding = 1, std430) readonly buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

const float MAX_ANCHOR_HEIGHT = 2.0;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  float flexiblity = data.material.z;
  float anchor_height = data.material.w;

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  vec3 position = vec3(0.0);

  if (flexiblity > 0.0) {
    position = wind_effect(world_position, in_position, scene.time, flexiblity, anchor_height, MAX_ANCHOR_HEIGHT);
  } else {
    position = world_position;
  }

  out_uv = in_uv;
  out_albedo_image_index = uint(data.image_indices.x);

  gl_Position = scene.light_space * vec4(position, 1.0);
}
