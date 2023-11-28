#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec4 in_light_space_position;
layout(location = 4) in vec4 in_tint;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  mat4 view;
  mat4 projection;
  vec3 camera_position;
  mat4 light_space;
  vec3 light_direction;
  vec4 light_color;
} scene;

layout(binding = 2) uniform sampler2D image;
layout(binding = 3) uniform sampler2D shadow_map;

layout(push_constant) uniform uniform_object {
  mat4 model;
  mat4 normal;
} object;

const material default_material = material(
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(1.0, 1.0, 1.0, 1.0),
  vec4(0.5, 0.5, 0.5, 1.0),
  32.0
);

float calculate_shadow(vec3 light_direction) {
  float shadow = 0.0;

  vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

  vec3 coordinates = in_light_space_position.xyz / in_light_space_position.w;

  if (coordinates.z > 1.0 || coordinates.z < -1.0) {
    return shadow;
  }

  float bias = max(0.001 * (1.0 - dot(in_normal, light_direction)), 0.0001);
  // float bias = 0.001;
  
  float current_depth = coordinates.z - bias;

  int count = 0;
  int range = 2;

  for (int x = -range; x <= range; ++x) {
    for (int y = -range; y <= range; ++y) {
      float pcf_depth = texture(shadow_map, coordinates.xy + vec2(x, y) * texel_size).r;
      shadow += current_depth > pcf_depth ? 1.0 : 0.0;
      ++count;
    }
  }

  return shadow / float(count);
}

void main() {
  vec3 light_direction = normalize(-scene.light_direction);

  // Ambient
  vec4 ambient_color = scene.light_color * 0.15;

  // Diffuse
  float diffuse_factor = max(dot(light_direction, in_normal), 0.0);
  vec4 diffuse_color = diffuse_factor * scene.light_color;

  // Specular
  vec3 view_direction = normalize(scene.camera_position - in_position);
  vec3 halfway_direction = normalize(light_direction + view_direction);  
  float specular_factor = pow(max(dot(in_normal, halfway_direction), 0.0), default_material.shininess);
  vec4 specular_color = specular_factor * scene.light_color; 

  // Calculate shadow
  float shadow_factor = calculate_shadow(light_direction);

  // Sample texture
  vec4 sampled_color = texture(image, in_uv);

  out_color = (ambient_color + (1.0 - shadow_factor) * (diffuse_color + specular_color)) * sampled_color * in_tint;
  // out_color = (ambient_color + diffuse_color + specular_color) * sampled_color * (1.0 - shadow_factor);
  // out_color = color * shadow;

  // out_color = vec4(shadow_factor, shadow_factor, shadow_factor, 1.0);
  // out_color = vec4(texture(shadow_map, in_uv).rrr, 1.0);
}
