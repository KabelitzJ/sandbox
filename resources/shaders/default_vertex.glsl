#version 330 core

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 color;

uniform mat4 uni_model;
uniform mat4 uni_view;
uniform mat4 uni_projection;

// out vec4 vertex_color;

void main() {
  gl_Position = uni_projection * uni_view * uni_model * vec4(position, 1.0);
  // vertex_color = vec4(color, 1.0f);
}
