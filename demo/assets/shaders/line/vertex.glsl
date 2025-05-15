#version 450 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(push_constant) uniform push_data {
	mat4 model;
} data;

void main() {
  vec3 world_position = vec3(data.model * vec4(in_position, 1.0));

  out_position = world_position;
  out_color = in_color;

  gl_Position = scene.projection * scene.view * vec4(world_position, 1.0);
}
