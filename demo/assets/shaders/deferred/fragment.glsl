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
layout(location = 8) in flat uint in_albedo_image_index;
layout(location = 9) in flat uint in_normal_image_index;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_position;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_material;
layout(location = 4) out float out_depth;

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

// layout(set = 0, binding = 1, std430) readonly buffer buffer_point_lights {
//   point_light data[];
// } point_lights;

// layout(set = 0, binding = 2) uniform sampler2D shadow_map_image;

layout(set = 0, binding = 1) uniform sampler images_sampler;
layout(set = 0, binding = 2) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

const vec4 AMBIENT_COLOR = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SPECULAR_COLOR = vec4(0.9, 0.9, 0.9, 1.0);
const float GLOSSINESS = 32.0;

const vec4 RIM_COLOR = vec4(1.0, 1.0, 1.0, 1.0);
const float RIM_STRENGTH = 0.716;
const float RIM_THRESHOLD = 0.1;

const vec4 FOG_COLOR = vec4(0.5, 0.5, 0.5, 1.0);
const float FOG_START = 250.0;
const float FOG_DENSITY = 0.002;

vec4 get_albedo() {
  if (in_albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  return texture(sampler2D(images[in_albedo_image_index], images_sampler), in_uv).rgba * in_color;
}

vec3 get_normal() {
  if (in_normal_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in_normal);
  }

  vec3 normal = texture(sampler2D(images[in_normal_image_index], images_sampler), in_uv).rgb * 2.0 - 1.0;

  return normalize(in_tbn * normal);
}

void main(void) {
  // vec4 albedo = get_albedo();

  // if (albedo.a < 0.8) {
  //   discard;
  // }

  vec4 albedo = vec4(in_uv, 0.0, 1.0);

  out_albedo = albedo;
  out_position = vec4(in_position, 1.0);
  out_normal = vec4(get_normal(), 0.0);
  out_material = vec4(in_material, 0.0, 0.0);
  out_depth = linearize_depth(gl_FragCoord.z, DEFAULT_NEAR, DEFAULT_FAR);
}
