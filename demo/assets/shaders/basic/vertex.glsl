#version 450

layout(location = 0) in vec2 vertex_in_position;
layout(location = 1) in vec4 vertex_in_color;

layout(location = 0) out vec4 vertex_out_color;

void main() {
  gl_Position = vec4(vertex_in_position, 0.0, 1.0);
  vertex_out_color = vertex_in_color;
}
