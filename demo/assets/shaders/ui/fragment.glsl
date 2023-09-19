#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_object {
  mat4 placeholder;
} object;

layout(binding = 2) uniform sampler2D image;

void main() {
  vec4 sampled = vec4(1.0, 0.0, 1.0, texture(image, in_uv).r);

  out_color = vec4(1.0, 1.0, 1.0, 1.0) * sampled;
}
