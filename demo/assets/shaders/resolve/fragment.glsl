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

layout(binding = 1, input_attachment_index = 0) uniform subpassInput position_image; 
layout(binding = 2, input_attachment_index = 1) uniform subpassInput normal_image;
layout(binding = 3, input_attachment_index = 2) uniform subpassInput albedo_image;

layout(binding = 4) uniform sampler2D shadow_map_image;

const material DEFAULT_MATERIAL = material(
  vec4(1.0, 1.0, 1.0, 1.0),   // Ambient color
  vec4(1.0, 1.0, 1.0, 1.0),   // Diffuse color
  vec4(0.5, 0.5, 0.5, 1.0),   // Specular color
  64.0                        // Shininess
);

const mat4 DEPTH_BIAS = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

void main() {
  vec3 position = subpassLoad(position_image).xyz;
  vec3 normal = subpassLoad(normal_image).xyz;
  vec4 albedo = subpassLoad(albedo_image);

  vec4 light_space_position = (DEPTH_BIAS * scene.light_space) * vec4(position, 1.0);

  vec3 view_direction = normalize(scene.camera_position - position);
  
  directional_light light = directional_light(scene.light_direction, scene.light_color);

  light_result light_result = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, light, normal, view_direction);

  // float shadow_factor = calculate_shadow_pcf(shadow_map_image, light_space_position, normal, light.direction);
  float shadow_factor = calculate_shadow_random_jitter(shadow_map_image, light_space_position, normal, light.direction);

  vec4 lighting = light_result.ambient + (light_result.diffuse + light_result.specular) * shadow_factor;

  out_color = albedo * lighting;
  // out_color = vec4(vec3(texture(shadow_map_image, in_uv).r), 1.0);
  // out_color = vec4(vec3(texture(depth_image, in_uv).r), 1.0);
  // out_color = vec4(normal, 1.0);
}
