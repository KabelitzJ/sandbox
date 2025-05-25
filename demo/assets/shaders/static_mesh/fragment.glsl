#version 450 core

#include <libsbx/common/lighting.glsl>
#include <libsbx/common/material.glsl>
#include <libsbx/common/random.glsl>
#include <libsbx/common/depth.glsl>
#include <libsbx/common/shadow.glsl>

// #define ENABLE_SHADOWS 0

#define MAX_IMAGE_ARRAY_SIZE 64

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec2 in_material;
layout(location = 5) in flat uint in_albedo_image_index;
layout(location = 6) in flat uint in_normal_image_index;

layout(location = 0) out vec4 out_color;

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

  vec3 tangent_normal = texture(sampler2D(images[in_normal_image_index], images_sampler), in_uv).xyz * 2.0 - 1.0;

  vec3 Q1 = dFdx(in_position);
  vec3 Q2 = dFdy(in_position);
  vec2 st1 = dFdx(in_uv);
  vec2 st2 = dFdy(in_uv);

  vec3 N = normalize(in_normal);
  vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangent_normal);
}

void main(void) {
  vec3 world_position = in_position;
  vec4 albedo = get_albedo();

  if (albedo.a < 0.5) {
    discard;
  }

  vec3 normal = get_normal();

  float metallic = in_material.x;
  float roughness = in_material.y;

  vec3 N = normalize(normal);
  vec3 L = normalize(-scene.light_direction);
  vec3 V = normalize(scene.camera_position - world_position);
  vec3 H = normalize(L + V);
  
  float diffuse_strength = max(dot(N, L), 0.0);
  float specular_strength = pow(max(dot(N, H), 0.0), 16.0);

  vec4 ambient = AMBIENT_COLOR * albedo;
  vec4 diffuse = diffuse_strength * albedo * scene.light_color;
  vec4 specular = SPECULAR_COLOR * specular_strength * albedo;

  vec4 color = ambient + diffuse + specular;

  out_color = color;
}
