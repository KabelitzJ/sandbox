#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;
layout(location = 2) out vec3 out_normal;

layout(binding = 0) uniform buffer_object {
  mat4 model;
  mat4 view;
  mat4 projection;
} uniform_buffer_object;

void main() {
  mat4 inverse_model = transpose(inverse(uniform_buffer_object.model));

  out_position = vec3(uniform_buffer_object.model * vec4(in_position, 1.0));
  out_color = in_color;
  // out_normal = normalize(mat3(inverse_model) * in_normal);
  out_normal = in_normal;

  gl_Position = uniform_buffer_object.projection * uniform_buffer_object.view * vec4(out_position, 1.0);
}
