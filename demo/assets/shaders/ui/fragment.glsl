#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_object {
  vec4 color;
} object;

layout(binding = 2) uniform sampler2D atlas;

void main() {
  out_color = vec4(1.0, 1.0, 1.0, texture(atlas, in_uv).r) * object.color;
}
