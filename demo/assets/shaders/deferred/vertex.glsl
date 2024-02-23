#version 450

#include "../common/wind.glsl"
#include "../common/constants.glsl"

struct per_mesh_data {
  mat4 model;
  mat4 normal;
  vec4 tint;
  vec4 wind; // x = flexibility, y = anchor height, z = unused, w = unused
}; // struct per_mesh_data

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  float time;
} scene;

layout(binding = 1) buffer buffer_mesh_data {
  per_mesh_data data[];
} mesh_data;

const float MAX_ANCHOR_HEIGHT = 2.0;
const float BRIGHTNESS_EFFECT = 0.5;

void main() {
  const per_mesh_data data = mesh_data.data[gl_InstanceIndex];

  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  // out_position = wind_effect(world_position, in_position, scene.time data.wind.x, data.wind.y, MAX_ANCHOR_HEIGHT);
  out_position = world_position;
  out_normal = normalize(vec3(data.normal * vec4(in_normal, 1.0)));
  out_uv = in_uv;

  float brightness = (data.tint.r + data.tint.g + data.tint.b) / 3.0;
  brightness = (1.0 - BRIGHTNESS_EFFECT) + BRIGHTNESS_EFFECT * brightness;
  out_color = vec4(data.tint.rgb * brightness, 1.0);
  // out_color = data.tint;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
