#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

void main() {
  gl_Position = uni_projection * uni_view * uni_model * vec4(position, 1.0);
}
