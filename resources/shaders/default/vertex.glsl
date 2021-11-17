#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in mat4 model_matrix;

// uniform vertex_uniforms {
//   mat4 view_matrix;
//   mat4 projection_matrix;
// } uniforms;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

// out vertex_data {
//   vec2 uv;
// } out_data;

void main() {
  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
  // out_data.uv = uv;
}
