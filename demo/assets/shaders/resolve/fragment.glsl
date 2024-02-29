#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
} scene;

layout(binding = 1) uniform sampler2D position_image; 
layout(binding = 2) uniform sampler2D normal_image;
layout(binding = 3) uniform sampler2D albedo_image;
layout(binding = 4) uniform sampler2D shadow_map_image;

const material DEFAULT_MATERIAL = material(
  vec4(0.2, 0.2, 0.2, 1.0),   // Ambient color
  vec4(0.8, 0.8, 0.8, 1.0),   // Diffuse color
  vec4(1.0, 1.0, 1.0, 1.0),   // Specular color
  16.0                         // Shininess
);

void main() {
  vec3 position = texture(position_image, in_uv).xyz;
  vec3 normal = texture(normal_image, in_uv).xyz;
  vec4 albedo = texture(albedo_image, in_uv);
  vec2 shadow = texture(shadow_map_image, in_uv).rg;

  vec3 view_direction = normalize(scene.camera_position - position);
  
  directional_light light = directional_light(scene.light_direction, scene.light_color);

  vec4 lighting = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, light, normal, view_direction);

  out_color = albedo * lighting;
  // out_color = vec4(shadow, 0.0, 1.0);
}
