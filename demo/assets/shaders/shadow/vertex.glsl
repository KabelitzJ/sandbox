#version 450

layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform uniform_scene {
  mat4 light_space;
} scene;

layout(push_constant) uniform uniform_object {
  mat4 model;
} object;

void main() {
  gl_Position = scene.light_space * object.model * vec4(in_position, 1.0);
}
