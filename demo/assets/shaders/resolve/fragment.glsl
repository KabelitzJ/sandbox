#version 450

#include "../common/lighting.glsl"
#include "../common/material.glsl"
#include "../common/shadow.glsl"

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_scene {
  vec3 camera_position;
  vec3 light_direction;
  vec4 light_color;
  mat4 light_space;
  float time;
} scene;

layout(binding = 1) uniform sampler2D position_image; 
layout(binding = 2) uniform sampler2D normal_image;
layout(binding = 3) uniform sampler2D albedo_image;

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

  vec4 lighting = calculate_directional_light_blinn_phong(DEFAULT_MATERIAL, light, normal, view_direction);

  out_color = albedo * lighting;
}
