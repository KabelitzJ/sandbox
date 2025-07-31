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
layout(location = 7) in vec3 in_material;
layout(location = 8) in flat uvec3 in_image_indices;
layout(location = 9) in flat uvec2 in_object_id;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_position;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_material;
layout(location = 4) out uvec2 out_object_id;
layout(location = 5) out float out_depth;

layout(set = 0, binding = 1) uniform sampler images_sampler;
layout(set = 0, binding = 2) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

vec4 get_albedo() {
  uint albedo_image_index = in_image_indices.x;

  if (albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  vec4 color = texture(sampler2D(images[albedo_image_index], images_sampler), in_uv).rgba;

  return vec4(color.rgb * in_color.rgb, 1.0);
}

vec3 get_normal() {
  uint normal_image_index = in_image_indices.y;

  if (normal_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in_normal);
  }

  vec3 normal = texture(sampler2D(images[normal_image_index], images_sampler), in_uv).xyz;

  return normalize(in_tbn * normal);
}

vec3 get_material() {
  uint material_image_index = in_image_indices.z;

  if (material_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_material;
  }

  return texture(sampler2D(images[material_image_index], images_sampler), in_uv).rgb;
}

void main(void) {
  out_albedo = get_albedo();
  out_position = vec4(in_position, 1.0);
  out_normal = vec4(get_normal(), 0.0);
  out_material = vec4(get_material(), 0.0);
  out_object_id = in_object_id;
  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
