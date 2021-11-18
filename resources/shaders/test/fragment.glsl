#version 460 core

in vertex_data {
  vec4 color;
} in_data;

out vec4 out_color;

void main() {
  out_color = in_data.color;
}
