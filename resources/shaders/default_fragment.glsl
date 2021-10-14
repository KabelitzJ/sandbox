#version 330 core

in vec2 vertex_uv;

uniform vec4 uni_color;
uniform sampler2D uni_texture;

out vec4 fragment_color;
  
void main() {
  fragment_color = texture(uni_texture, vertex_uv) * uni_color;
}
