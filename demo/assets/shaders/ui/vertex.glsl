#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_uv;

layout(binding = 0) uniform uniform_scene {
  mat4 projection;
} scene;

layout(binding = 1) buffer buffer_transforms {
  mat4 data[];
} transforms;

void main() {
  out_uv = in_uv;

  gl_Position = scene.projection * transforms.data[gl_InstanceIndex] * vec4(in_position, 0.0, 1.0);
}
