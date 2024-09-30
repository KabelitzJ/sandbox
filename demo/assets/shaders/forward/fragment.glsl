#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/random.glsl"
#include "../common/depth.glsl"
#include "../common/shadow.glsl"

#define MAX_IMAGE_ARRAY_SIZE 64

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_color;
layout(location = 4) in flat uint in_albedo_image_index;
layout(location = 5) in flat uint in_normal_image_index;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  uint point_light_count;
} scene;

layout(binding = 2, std430) readonly buffer buffer_point_lights {
  point_light data[];
} point_lights;

layout(binding = 3) uniform sampler2D shadow_map_image;

layout(binding = 4) uniform sampler images_sampler;
layout(binding = 5) uniform texture2D images[MAX_IMAGE_ARRAY_SIZE];

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),     // Ambient color
  vec4(1.0, 1.0, 1.0, 1.0),     // Specular color
  0.4,                          // Metallic
  0.7,                          // Roughness
  0.7                           // Ambient occlusion
);

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

vec3 get_normal() {
  if (in_normal_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return normalize(in_normal);
  }

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

vec4 get_albedo() {
  if (in_albedo_image_index >= MAX_IMAGE_ARRAY_SIZE) {
    return in_color;
  }

  return texture(sampler2D(images[in_albedo_image_index], images_sampler), in_uv) * in_color;
}

void main(void) {
  vec3 world_position = in_position;
  vec3 normal = get_normal();
  vec4 albedo = get_albedo();

  vec4 light_space_position = DEPTH_BIAS * scene.light_space * vec4(world_position, 1.0);

  float shadow = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, scene.light_direction);

  directional_light light = directional_light(scene.light_direction, scene.light_color);

  light_result result = calculate_directional_light(light, world_position, normal, scene.camera_position, DEFAULT_MATERIAL);

  out_color = albedo * (result.ambient + result.diffuse + result.specular);

  // vec3 light_direction = normalize(-scene.light_direction);
  // vec3 view_direction = normalize(scene.camera_position - world_position);
  // vec3 halfway_direction = normalize(light_direction + view_direction);

  // float n_dot_l = dot(normal, halfway_direction);
  // float light_intensity = smoothstep(0, 0.01, n_dot_l * shadow);

  // vec4 light = scene.light_color * light_intensity;

  // float specular_factor = pow(n_dot_l * light_intensity, GLOSSINESS * GLOSSINESS);
  // float specular_intensity = smoothstep(0.005, 0.01, specular_factor);
  // vec4 specular = SPECULAR_COLOR * specular_intensity;

  // float rim_factor = (1.0 - dot(view_direction, normal)) * pow(n_dot_l, RIM_THRESHOLD);
  // float rim_intensity = smoothstep(RIM_STRENGTH - 0.01, RIM_STRENGTH + 0.01, rim_factor);
  // vec4 rim = RIM_COLOR * rim_intensity;

  // out_color = albedo * (AMBIENT_COLOR + light + specular + rim); // + light + specular + rim);
}
