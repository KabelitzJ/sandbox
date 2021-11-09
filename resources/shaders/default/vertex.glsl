#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform vertex_uniforms {
  mat4 model_matrix;
  mat4 view_matrix;
  mat4 projection_matrix;
} uniforms;

out vertex_data {
  vec2 uv;
} out_data;

void main() {
  gl_Position = uniforms.projection_matrix * uniforms.view_matrix * uniforms.model_matrix * vec4(position, 1.0);
  out_data.uv = uv;
}
