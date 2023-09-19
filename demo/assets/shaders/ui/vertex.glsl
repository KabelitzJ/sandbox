#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_uv;

layout(binding = 0) uniform uniform_object {
  mat4 placeholder;
} object;

layout(binding = 1) uniform uniform_scene {
  mat4 projection;
} scene;

void main() {
  out_uv = in_uv;

  gl_Position = scene.projection * object.placeholder * vec4(in_position, 0.0, 1.0);
}
