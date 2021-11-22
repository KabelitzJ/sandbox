#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vertex_data {
  vec2 uv;
} out_data;

void main() {
  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
  out_data.uv = uv;
}
