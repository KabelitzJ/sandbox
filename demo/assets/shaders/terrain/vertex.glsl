#version 460 core

#include <libsbx/common/constants.glsl>


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec4 out_water_color;
layout(location = 4) out vec4 out_land_color;
layout(location = 5) out vec4 out_mountain_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(push_constant) uniform push_data {
	mat4 model;
  mat4 normal;
  vec4 water_color;
  vec4 land_color;
  vec4 mountain_color;
} push;

void main() {
  vec3 world_position = vec3(push.model * vec4(in_position, 1.0));

  out_position = world_position;
  out_normal = normalize(vec3(push.normal * vec4(in_normal, 1.0)));
  out_uv = in_uv;

  out_water_color = push.water_color;
  out_land_color = push.land_color;
  out_mountain_color = push.mountain_color;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
