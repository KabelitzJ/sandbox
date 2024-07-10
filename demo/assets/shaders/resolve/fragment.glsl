#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/shadow.glsl"
#include "../common/depth.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
} scene;

layout(binding = 1) uniform sampler2D position_image; 
layout(binding = 2) uniform sampler2D normal_image;
layout(binding = 3) uniform sampler2D albedo_image;
layout(binding = 4) uniform sampler2D shadow_map_image;
layout(binding = 5) uniform sampler2D depth_image;

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),   // Ambient color
  vec4(1.0, 1.0, 1.0, 1.0),   // Diffuse color
  vec4(0.5, 0.5, 0.5, 1.0),   // Specular color
  32.0                        // Shininess
);

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

void main() {
  vec3 position = texture(position_image, in_uv).xyz;
  vec4 light_space_position = (DEPTH_BIAS * scene.light_space) * vec4(position, 1.0);
  vec3 normal = texture(normal_image, in_uv).xyz;
  vec4 albedo = texture(albedo_image, in_uv);

  vec3 view_direction = normalize(scene.camera_position - position);
  
  directional_light light = directional_light(scene.light_direction, scene.light_color);

  blinn_phong_result lighting_result = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, light, normal, view_direction);

  // float shadow_factor = calculate_shadow_pcf(shadow_map_image, light_space_position, normal, light.direction);
  float shadow_factor = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, light.direction);

  vec4 lighting = lighting_result.ambient + (lighting_result.diffuse + lighting_result.specular) * shadow_factor;

  out_color = albedo * lighting;
  // out_color = vec4(vec3(texture(shadow_map_image, in_uv).r), 1.0);
  // out_color = vec4(vec3(texture(depth_image, in_uv).r), 1.0);
  // out_color = vec4(normal, 1.0);
}
