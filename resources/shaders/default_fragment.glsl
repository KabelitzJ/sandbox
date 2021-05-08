#version 330 core

in vec4 vertex_color;

uniform vec4 uni_color;

out vec4 fragment_color;
  
void main() {
  fragment_color = (vertex_color + uni_color) * 0.5f;
}
