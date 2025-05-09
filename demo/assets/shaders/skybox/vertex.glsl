#version 450

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(binding = 1) uniform uniform_object {
  mat4 model;
  vec4 tint;
} object;

layout(location = 0) out vec3 out_position;

void main() {
  mat4 view_no_translation = mat4(mat3(scene.view));

  vec4 position = scene.projection * view_no_translation * vec4(in_position, 1.0);

  gl_Position = position.xyww;

  out_position = in_position;
}
