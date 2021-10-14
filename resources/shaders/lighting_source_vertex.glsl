#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 uni_model_matrix;
uniform mat4 uni_view_matrix;
uniform mat4 uni_projection_matrix;

void main() {
  gl_Position = uni_projection_matrix * uni_view_matrix * uni_model_matrix * vec4(position, 1.0);
}
