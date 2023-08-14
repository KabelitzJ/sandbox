#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

layout(binding = 0) uniform uniform_object {
  mat4 model;
  mat4 normal;
} object;

layout(binding = 1) uniform uniform_scene {
  mat4 view;
  mat4 projection;
} scene;

void main() {
  out_position = vec3(object.model * vec4(in_position, 1.0));
  out_normal = normalize(mat3(object.normal) * in_normal);
  out_uv = in_uv;

  gl_Position = scene.projection * scene.view * vec4(out_position, 1.0);
}
