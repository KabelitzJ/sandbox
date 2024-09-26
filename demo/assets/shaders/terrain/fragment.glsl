#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"
#include "../common/depth.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;
layout(location = 3) out float out_depth;

layout(binding = 2) uniform sampler2D grass_albedo_image;
layout(binding = 4) uniform sampler2D dirt_albedo_image;

void main(void) {
  out_position = vec4(in_position, 1.0);

  // float noise = clamp(fbm(in_uv / 20.0, vec2(8.0), 16, 0.0, 0.0, 0.5, 2.0, -0.5, 8502384.237552), 0.0, 1.0);

  vec4 grass_albedo = texture(grass_albedo_image, in_uv);
  vec4 dirt_albedo = texture(dirt_albedo_image, in_uv);

  out_albedo = grass_albedo;
  out_normal = vec4(in_normal, 1.0);

  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
