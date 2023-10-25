#version 450

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(push_constant) uniform uniform_object {
  mat4 model;
} object;

void main() {
  gl_Position = scene.projection * scene.view * object.model * vec4(in_position, 1.0);
}
