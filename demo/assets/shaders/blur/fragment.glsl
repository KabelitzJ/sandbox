#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 1) uniform sampler2D image;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_data {
	vec2 direction;
} data;

void main() {
  vec2 image_size = vec2(textureSize(image, 0));

  vec4 color = vec4(0.0);

  vec2 offset1 = 1.3846153846f * data.direction;
  vec2 offset2 = 3.2307692308f * data.direction;

  color += texture(image, in_uv) * 0.2270270270f;
  color += texture(image, in_uv + (offset1 / image_size)) * 0.3162162162f;
  color += texture(image, in_uv - (offset1 / image_size)) * 0.3162162162f;
  color += texture(image, in_uv + (offset2 / image_size)) * 0.0702702703f;
  color += texture(image, in_uv - (offset2 / image_size)) * 0.0702702703f;

  out_color = color;
}
