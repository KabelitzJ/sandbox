#version 330 core

in vec2 texture_coordinates;

uniform sampler2D text;
uniform vec3 color;

out vec4 fragment_color;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texture_coordinates).r);
  fragment_color = vec4(color, 1.0) * sampled;
}
