#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv;

layout(binding = 0) uniform buffer_object {
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
} uniform_buffer_object;

void main() {
  out_position = vec3(uniform_buffer_object.model * vec4(in_position, 1.0));
  out_normal = normalize(mat3(uniform_buffer_object.normal) * in_normal);
  out_uv = in_uv;

  gl_Position = uniform_buffer_object.projection * uniform_buffer_object.view * vec4(out_position, 1.0);
}
