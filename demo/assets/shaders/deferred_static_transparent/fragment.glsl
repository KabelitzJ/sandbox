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

layout(location = 0) out vec4 out_accumulation;
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

/// @param color Regular RGB reflective color of fragment, not pre-multiplied
/// @param alpha Alpha value of fragment
/// param wsZ Window-space-z value == gl_FragCoord.z
void write_pixel(vec3 color, float alpha, float wsZ) {
  float ndcZ = 2.0 * wsZ - 1.0;

  // linearize depth for proper depth weighting
  //See: https://stackoverflow.com/questions/7777913/how-to-render-depth-linearly-in-modern-opengl-with-gl-fragcoord-z-in-fragment-sh
  //or: https://stackoverflow.com/questions/11277501/how-to-recover-view-space-position-given-view-space-depth-value-and-ndc-xy
  float linearZ = (scene.projection[2][2] + 1.0) * wsZ / (scene.projection[2][2] + ndcZ);
  float tmp = (1.0 - linearZ) * alpha;

  //float tmp = (1.0 - wsZ * 0.99) * alpha * 10.0; // <-- original weighting function from paper #2
  float w = clamp(tmp * tmp * tmp * tmp * tmp * tmp, 1e-5, 1e4);

  out_accumulation = vec4(color * alpha* w, alpha);
  out_revealage = alpha * w;
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

  write_pixel(albedo.rgb, albedo.a, gl_FragCoord.z);
}
