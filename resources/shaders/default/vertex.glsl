#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in mat4 model_matrix;

uniform matrices {
  mat4 view_matrix;
  mat4 projection_matrix;
} uniforms;

void main() {
  gl_Position = uniforms.projection_matrix * uniforms.view_matrix * model_matrix * vec4(position, 1.0);
}
