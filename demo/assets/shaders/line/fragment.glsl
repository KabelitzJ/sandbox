#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_color;

void main(void) {
  out_position = vec4(in_position, 0.0);
  out_color = in_color;
}
