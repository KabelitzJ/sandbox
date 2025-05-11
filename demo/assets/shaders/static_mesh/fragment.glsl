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

#define TRANSPARENCY_DISABLED 0
#define TRANSPARENCY_ENABLED 1

layout(constant_id = 0) const uint transparency = TRANSPARENCY_DISABLED;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
  float time;
} scene;

layout(binding = 2, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(binding = 3) uniform sampler2D shadow_map_image;

layout(binding = 4) uniform sampler images_sampler;
layout(binding = 5) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

const vec4 AMBIENT_COLOR = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 SPECULAR_COLOR = vec4(0.9, 0.9, 0.9, 1.0);
const float GLOSSINESS = 32.0;
const vec4 RIM_COLOR = vec4(1.0, 1.0, 1.0, 1.0);
const float RIM_STRENGTH = 0.716;
const float RIM_THRESHOLD = 0.1;

const vec4 FOG_COLOR = vec4(0.5, 0.5, 0.5, 1.0);
const float FOG_START = 250.0;
const float FOG_DENSITY = 0.000;

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

vec4 get_albedo() {
  if (in_albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  return texture(sampler2D(images[in_albedo_image_index], images_sampler), in_uv).rgba * in_color;
}

void main(void) {
  vec3 world_position = in_position;
  vec3 normal = get_normal();
  vec4 albedo = get_albedo();

  if (transparency == TRANSPARENCY_DISABLED && albedo.a < 0.5) {
    discard;
  }

  float metallic = in_material.x;
  float roughness = in_material.y;

  // vec4 light_space_position = DEPTH_BIAS * scene.light_space * vec4(world_position, 1.0);

  // #ifdef ENABLE_SHADOWS
  //   float shadow = calculate_shadow_pcf(shadow_map_image, light_space_position, normal, scene.light_direction);
  // #else
  //   float shadow = 1.0;
  // #endif

  float distance_to_camera = length(world_position - scene.camera_position);

  float adjusted_distance = max(0.0, distance_to_camera - FOG_START);

  float fog_factor = clamp(1.0 - exp(-adjusted_distance * FOG_DENSITY), 0.0, 1.0);

  vec3 light_position = normalize(-scene.light_direction);

  float n_dot_l = dot(light_position, normal);

  float light_intensity = smoothstep(0.0, 0.01, n_dot_l); // add shadow here (ndot_l * shadow)
  vec4 light = scene.light_color * light_intensity;

  vec3 view_direction = normalize(scene.camera_position - world_position);
  vec3 half_direction = normalize(light_position + view_direction);
  float n_dot_h = dot(normal, half_direction);

  float specular_intensity = smoothstep(0.005, 0.01, pow(n_dot_h * light_intensity, (1.0 - roughness) * GLOSSINESS * GLOSSINESS));
  vec4 specular_color = mix(SPECULAR_COLOR, albedo, metallic);
  vec4 specular = specular_color * specular_intensity;

  float rim_intensity = smoothstep(RIM_STRENGTH - 0.01, RIM_STRENGTH + 0.01, (1.0 - dot(normal, view_direction)) * pow(n_dot_l, RIM_THRESHOLD)) * (1.0 - roughness);
  vec4 rim = RIM_COLOR * rim_intensity;

  if (transparency == TRANSPARENCY_ENABLED) {
    out_color = vec4(vec3(albedo * (AMBIENT_COLOR + light + specular + rim)), albedo.a);
  } else {
    out_color = vec4(vec3(albedo * (AMBIENT_COLOR + light + specular + rim)), 1.0);
  }

  // out_color = mix(out_color, FOG_COLOR, fog_factor);
  // out_color = vec4(vec3(albedo.r), 1.0);
  // out_color = vec4(vec3(albedo), 0.5);
  // out_color = vec4(vec3(albedo.a), 1.0);
  // out_color = albedo;
}
