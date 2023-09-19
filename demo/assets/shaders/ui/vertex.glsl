#version 450

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_position;

void main() {
  gl_Position = vec4(out_position, 0.0, 1.0);
}
