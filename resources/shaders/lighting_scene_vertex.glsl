#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

out vec2 vertex_uv;
out vec3 vertex_normal;
out vec3 vertex_fragment_position;

void main() {
  gl_Position = uni_projection * uni_view * uni_model * vec4(position, 1.0);
  vertex_uv = uv;
  vertex_normal = mat3(transpose(inverse(uni_model))) * normal;
  vertex_fragment_position = vec3(uni_model * vec4(position, 1.0));
}
