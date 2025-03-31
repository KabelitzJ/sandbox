#version 450

#define MAX_IMAGE_ARRAY_SIZE 64

layout(location = 1) in vec2 in_uv;
layout(location = 2) in flat uint in_albedo_image_index;

layout(location = 0) out float out_color;

layout(binding = 2) uniform sampler images_sampler;
layout(binding = 3) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

vec4 get_albedo() {
  if (in_albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return vec4(1.0, 1.0, 1.0, 1.0);
  }

  return texture(sampler2D(images[in_albedo_image_index], images_sampler), in_uv).rgba;
}

void main() {
  vec4 albedo = get_albedo();

  if (albedo.a < 0.01) {
    discard;
  }

  out_color = gl_FragCoord.z;

  // float dx = dFdx(depth);
  // float dy = dFdy(depth);

  // float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);

  // out_color = vec2(depth, 0.0);
}
