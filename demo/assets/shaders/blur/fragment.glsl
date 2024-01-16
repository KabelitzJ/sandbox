#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 1) uniform sampler2D color_texture;

layout(binding = 0) uniform writeonly image2D write_texture;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform push_scene {
	vec2 direction;
} scene;

void main() {
  vec2 color_texture_size = vec2(textureSize(color_texture, 0));

  vec4 color = vec4(0.0);

  vec2 offset1 = 1.3846153846f * scene.direction;
  vec2 offset2 = 3.2307692308f * scene.direction;

  color += texture(color_texture, in_uv) * 0.2270270270f;
  color += texture(color_texture, in_uv + (offset1 / color_texture_size)) * 0.3162162162f;
  color += texture(color_texture, in_uv - (offset1 / color_texture_size)) * 0.3162162162f;
  color += texture(color_texture, in_uv + (offset2 / color_texture_size)) * 0.0702702703f;
  color += texture(color_texture, in_uv - (offset2 / color_texture_size)) * 0.0702702703f;

  imageStore(write_texture, ivec2(in_uv * imageSize(write_texture)), color);

  out_color = color;
}
