#version 450 core

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"
#include "../common/depth.glsl"

#define MAX_IMAGE_ARRAY_SIZE 128

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;
layout(location = 4) in flat uint in_albedo_image_index;
layout(location = 5) in flat uint in_normal_image_index;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;
layout(location = 3) out float out_depth;

layout(binding = 2) uniform sampler images_sampler;
layout(binding = 3) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

vec3 get_normal_from_map() {
  vec3 tangent_normal = texture(sampler2D(images[in_normal_image_index], images_sampler), in_uv).xyz * 2.0 - 1.0;

  vec3 Q1  = dFdx(in_position);
  vec3 Q2  = dFdy(in_position);
  vec2 st1 = dFdx(in_uv);
  vec2 st2 = dFdy(in_uv);

  vec3 N   = normalize(in_normal);
  vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B  = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangent_normal);
}

void main(void) {
  out_position = vec4(in_position, 1.0);
  
  if (in_albedo_image_index < MAX_IMAGE_ARRAY_SIZE) {
    out_albedo = texture(sampler2D(images[in_albedo_image_index], images_sampler), in_uv) * in_color;
  } else {
    out_albedo = in_color;
  }

  if (in_normal_image_index < MAX_IMAGE_ARRAY_SIZE) {
    out_normal = vec4(get_normal_from_map(), 1.0);
  } else {
    out_normal = vec4(in_normal, 1.0);
  }

  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
