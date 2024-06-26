#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

void main() {
  out_position = in_position;
  out_color = in_color;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
