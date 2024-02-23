#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D position;
layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D color;

void main() {
  out_color = texture(color, in_uv);
}
