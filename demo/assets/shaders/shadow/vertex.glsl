#version 450

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_position;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

layout(push_constant) uniform uniform_object {
  mat4 model;
} object;

void main() {
  out_position = vec3(object.model * vec4(in_position, 1.0));

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
