#version 460 core

uniform sampler2D texture;

in vertex_data {
  vec2 uv;
} in_data;

out vec4 out_color;

void main() {
  out_color =  texture(texture, in_data.uv);
}
