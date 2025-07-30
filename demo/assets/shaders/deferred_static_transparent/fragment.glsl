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

layout(location = 0) out vec4 out_accum;
layout(location = 1) out float out_revealage;

layout(set = 0, binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  // uint point_light_count;
  float time;
} scene;

layout(set = 0, binding = 1) uniform sampler images_sampler;
layout(set = 0, binding = 2) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

vec4 get_albedo() {
  uint albedo_image_index = in_image_indices.x;

  if (albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  return texture(sampler2D(images[albedo_image_index], images_sampler), in_uv).rgba * in_color;
}

vec3 get_normal() {
  uint normal_image_index = in_image_indices.y;

  if (normal_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in_normal);
  }

  vec3 normal = texture(sampler2D(images[normal_image_index], images_sampler), in_uv).xyz;

  return normalize(in_tbn * normal);
}

void main(void) {
  vec4 albedo = get_albedo();

  float weight = clamp(pow(min(1.0, albedo.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

  out_accum = vec4(albedo.rgb * albedo.a, albedo.a) * weight;
  out_revealage = albedo.a;
}
