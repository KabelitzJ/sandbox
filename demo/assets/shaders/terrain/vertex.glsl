#version 450

#include "../common/constants.glsl"


layout(location = 0) in vec2 in_position;
layout(location = 1) in uint in_index;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(binding = 1) readonly buffer buffer_heightmap {
  float data[];
} heightmap;

layout(push_constant) uniform push_data {
	mat4 model;
  mat4 normal;
  vec4 tint;
} data;

void main() {
  vec3 position = vec3(in_position.x, heightmap.data[in_index], in_position.y);

  vec3 world_position = vec3(data.model * vec4(position, 1.0));

  out_position = world_position;
  out_normal = normalize(vec3(data.normal * vec4(in_normal, 1.0)));
  out_uv = in_uv;

  out_color = data.tint;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}