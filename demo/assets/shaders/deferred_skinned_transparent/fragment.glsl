#version 460 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/material.glsl>
#include <libsbx/common/random.glsl>
#include <libsbx/common/depth.glsl>
#include <libsbx/common/shadow.glsl>

// #define ENABLE_SHADOWS 0

#define MAX_IMAGE_ARRAY_SIZE 64

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in mat3 in_tbn; // Needs 3 locations slots (2, 3, 4)
layout(location = 5) in vec2 in_uv;
layout(location = 6) in vec4 in_color;
layout(location = 7) in vec2 in_material;
layout(location = 8) in flat uvec2 in_image_indices;
layout(location = 9) in flat uvec2 in_object_id;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out float out_alpha;
layout(location = 2) out vec4 out_position;
layout(location = 3) out vec4 out_normal;
layout(location = 4) out vec4 out_material;
layout(location = 5) out uvec2 out_object_id;
layout(location = 6) out float out_depth;

layout(set = 0, binding = 1) uniform sampler images_sampler;
layout(set = 0, binding = 2) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

vec4 get_albedo() {
  uint albedo_image_index = in_image_indices.x;

  if (albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  vec4 color = texture(sampler2D(images[albedo_image_index], images_sampler), in_uv).rgba;

  return color * in_color;
}

vec3 get_normal() {
  uint normal_image_index = in_image_indices.y;

  if (normal_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in_normal);
  }

  vec3 normal = texture(sampler2D(images[normal_image_index], images_sampler), in_uv).rgb * 2.0 - 1.0;

  return normalize(in_tbn * normal);
}

void main(void) {
  vec4 albedo = get_albedo();

  // float weight = max(min(1.0, max(max(albedo.r, albedo.g), albedo.b) * albedo.a), albedo.a);
  float weight = clamp(pow(min(1.0, albedo.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

  out_albedo = vec4(albedo.rgb * albedo.a, albedo.a) * weight;
  out_alpha = albedo.a;
  out_position = vec4(in_position, 1.0);
  out_normal = vec4(get_normal(), 0.0);
  out_material = vec4(in_material, 0.0, 0.0);
  out_object_id = in_object_id;
  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
