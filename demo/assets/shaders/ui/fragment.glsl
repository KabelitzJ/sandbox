#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 2) uniform uniform_object {
  vec4 color;
} object;

layout(binding = 3) uniform sampler2D atlas;

void main() {
  float opacity = texture(atlas, in_uv).r;

  out_color = object.color * vec4(1.0, 1.0, 1.0, 1.0);
}
