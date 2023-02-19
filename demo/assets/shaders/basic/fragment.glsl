#version 450

layout(location = 0) in vec4 vertex_out_color;

layout(location = 0) out vec4 fragment_out_color;

void main() {
  fragment_out_color = vertex_out_color; 
}
