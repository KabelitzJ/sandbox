#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 uni_model_matrix;
uniform mat4 uni_view_matrix;
uniform mat4 uni_projection_matrix;
uniform mat3 uni_normal_matrix;

out vec2 vertex_uv;
out vec3 vertex_normal;
out vec3 vertex_fragment_position;

void main() {
  gl_Position = uni_projection_matrix * uni_view_matrix * uni_model_matrix * vec4(position, 1.0);
  vertex_uv = uv;
  vertex_normal = uni_normal_matrix * normal;
  vertex_fragment_position = vec3(uni_model_matrix * vec4(position, 1.0));
}
