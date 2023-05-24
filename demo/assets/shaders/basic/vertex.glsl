#version 450

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform buffer_object {
  mat4 model;
  mat4 view;
  mat4 projection;
} uniform_buffer_object;

void main() {
  gl_Position = uniform_buffer_object.projection * uniform_buffer_object.view * uniform_buffer_object.model * in_position;
  out_color = in_color;
}
